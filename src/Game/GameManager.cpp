#include "GameManager.h"
#include "../../external/olc/olcPixelGameEngine.h"
#include "../Engine/AcademiaEngine.h"
#include <chrono>
#include <memory>
#include <mutex>
#include <random>

void GameManager::Initialize(AcademiaEngine* engineContext)
{
    _EngineContext = engineContext;

#ifdef ACADEMIA_EXAMPLE
	_Player.SetGameManager(this);
    _Player.SetPosition(olc::vf2d(0.0f, 0.0f));
    // Setup collision manager references
    _CollisionManager.SetPlayer(&_Player);

    // Initialize player collider via Player API
    _Player.InitializeCollision(&_CollisionManager);

    // CollisionManager can still infer ennemies from registered colliders, so we don't need to pass containers explicitly
    _CollisionManager.SetBullets(&_Player.GetBullets());
#endif
}

void GameManager::Update(float elapsedTime)
{
    StartGameLogic(elapsedTime);
    constexpr olc::Key rightKey = olc::Key::D;
    constexpr olc::Key leftKey = olc::Key::A;
    constexpr olc::Key upKey = olc::Key::W;
    constexpr olc::Key downKey = olc::Key::S;
    constexpr olc::Key spaceKey = olc::Key::SPACE;
	constexpr olc::Key shiftKey = olc::Key::SHIFT;
    const olc::HWButton moveRightButton = _EngineContext->GetKey(rightKey);
    const olc::HWButton moveLeftButton = _EngineContext->GetKey(leftKey);
    const olc::HWButton moveUpButton = _EngineContext->GetKey(upKey);
    const olc::HWButton moveDownButton = _EngineContext->GetKey(downKey);
    const olc::HWButton jumpButton = _EngineContext->GetKey(spaceKey);
	const olc::HWButton sneakButton = _EngineContext->GetKey(shiftKey);
    
    //Gameplay code
    //Movement handle
    float x = 0.0f;
    float y = 0.0f;

    // This is very good, I don't like input code, it looks ugly I find :p, but there is no better way than this. Good Job :) 
    if (moveRightButton.bHeld) { x += 1.0f; }
    if (moveLeftButton.bHeld)  { x -= 1.0f; }
    if (moveUpButton.bHeld)    { y += 1.0f; }
    if (moveDownButton.bHeld)  { y -= 1.0f; }
	if (sneakButton.bHeld) { x *= 0.2f; y *= 0.2f; }
    if (_EngineContext->GetMouse(0).bPressed) {
        _Player.SpawnBullet(*_EngineContext);
    }

    /* SPAWNER handled by PeriodicTimer started in Initialize() */
    // Process spawn requests signalled by timers for each spawner (skip if paused)
    if (!_SpawnersPaused) {
        for (size_t i = 0; i < _Spawners.size(); ++i) {
            if (_SpawnRequested[i] && _SpawnRequested[i]->exchange(false)) {
                // spawn on main thread to keep CollisionManager usage single-threaded
                _Spawners[i]->SpawnEnnemies(*_EngineContext, &_Player, &_CollisionManager);
            }
        }
    }

    // Normalize diagonal movement
    if (x != 0.0f && y != 0.0f) {
        x *= 0.5f;
        y *= 0.5f;
    }

    std::vector<float> direction = { x, y };

#ifdef ACADEMIA_EXAMPLE
    auto& bullets = _Player.GetBullets();
    for (auto& bullet : bullets) {
        bullet.Update(*_EngineContext, elapsedTime);
        bullet.Draw(*_EngineContext);
    }
    // Remove bullets marked for removal immediately so UI button hits register once
    _Player.UpdateBullets(*_EngineContext);

    // Only run enemy updates, player movement and collision during active gameplay
    // Keep bullets updating/drawing above so player can shoot UI buttons on the start screen
    if (_IsGameStarted && !_IsGameOver) {
        // iterate enemies for each spawner (each spawner protects its own container)
        for (auto& sp : _Spawners) {
            std::lock_guard<std::mutex> lk(sp->GetEnnemiesMutex());
            auto& enemys = sp->GetEnnemies();
            for (auto& enemyPtr : enemys)
            {
                auto& enemy = *enemyPtr;
                enemy.Update(*_EngineContext, elapsedTime);
                enemy.Draw(*_EngineContext);
            }
            Ennemies::RemoveEnnemie(enemys);
            _Player.AddScore(10.0f * enemys.size() * elapsedTime); // small score bonus for surviving more enemies
        }

        _Player.AddForce(*_EngineContext, 180.0f, direction, elapsedTime);
        _Player.DrawCursor(*_EngineContext, _Player.GetCursorPosition(*_EngineContext));
        _Player.Update(*_EngineContext, elapsedTime);
        _Player.Draw(*_EngineContext);

        // Now that bullets and enemies have moved this frame, run collision detection
        // Rebind bullet collider owners to current addresses in the deque to avoid dangling pointers
        auto& bulletsRef = _Player.GetBullets();
        for (auto& b : bulletsRef) {
            if (b.collider) {
                b.collider->owner = &b;
                b.collider->position = b.GetPosition();
                b.collider->size = b.GetRadius();
            }
        }
        _CollisionManager.SetBullets(&bulletsRef);
        _CollisionManager.Update();

        // collision logic handled here but bullets already cleaned up
    } else {
        // On start screen, still draw cursor so player can aim and shoot buttons
        _Player.DrawCursor(*_EngineContext, _Player.GetCursorPosition(*_EngineContext));
    }

    // UI code
    // Draw Player health bar
    _Player.GetPosition();
    olc::vi2d healthBarPos = _EngineContext->ConvertWorldPositionToPixels(_Player.GetPosition()) + olc::vi2d(-20, -30);
    _EngineContext->FillRect(healthBarPos, olc::vi2d(40, 5), olc::WHITE);
    if (_Player.GetHealth() > 0) {
        int healthWidth = static_cast<int>(40 * (_Player.GetHealth() / 100.0f));
        _EngineContext->FillRect(healthBarPos, olc::vi2d(healthWidth, 5), olc::GREEN);
    }
    DrawUI();
    _EngineContext->DrawString(10, 12, "FPS : " + std::to_string(_EngineContext->GetFPS()), alertUIYellow, 2);
    EndGameLogic(elapsedTime);
#endif
}

void GameManager::Uninitialize() {
    //detruit les new et pointeur que j'ai cr�er
    // stop and clear all spawner timers
    for (auto& t : _SpawnerTimers) {
        if (t) {
            t->stop();
        }
    }
    _SpawnerTimers.clear();

    _Player.ShutdownCollision(&_CollisionManager);
}

GameManager::~GameManager()
{
	Uninitialize();
}

void GameManager::DrawUI() {
	// Draw a simple white bar at the top of the screen for UI background
    _EngineContext->FillRect(0, 0, 1920, 35, bgColorNavyBlue);
    _EngineContext->DrawLine(0, 35, 1920, 35, mainUIOrange);
	int scoreTextWidth = _EngineContext->GetTextSize("Score: " + std::to_string(static_cast<int>(GetScore()))).x;
	_EngineContext->DrawString(1820 - scoreTextWidth, 12, "Score: " + std::to_string(static_cast<int>(GetScore())), alertUIYellow, 2);
    switch (static_cast<int>(_DifficultyLevel)) {
    case 0:
        _EngineContext->DrawString(830, 12, "Difficulty : EASY", alertUIYellow, 2);
        break;
    case 1:
        _EngineContext->DrawString(830, 12, "Difficulty : MEDIUM", alertUIYellow, 2);
        break;
    case 2:
        _EngineContext->DrawString(830, 12, "Difficulty : HARD", alertUIYellow, 2);
        break;
    }
}

void GameManager::SetupSpawner() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(3, 10);
    int random_num = distrib(gen);
    // Create multiple spawners
    int spawnerCount = 2 + static_cast<int>(_DifficultyLevel); // configurable number of spawners
    std::cout << "Difficulté : " << std::to_string(static_cast<int>(_DifficultyLevel));
    std::cout << "Nombre de spawner : " << spawnerCount;
    
    _Spawners.reserve(spawnerCount);
    // Note: reserve() only increases capacity, not size().
    // Do not access _Spawners by index before push_back; set positions on the spawner
    // object before moving it into the vector.
    _SpawnerTimers.reserve(spawnerCount);
    _SpawnRequested.resize(spawnerCount);

    for (int i = 0; i < spawnerCount; ++i) {
        auto sp = std::make_unique<Spawner>();
        // position spawners: start them off-screen depending on index, or default positions
        if (i == 0) {
            sp->SetPosition(olc::vf2d(_EngineContext->ScreenWidth()/2, _EngineContext->ScreenHeight()/2));
        }
        else if (i == 1) {
            sp->SetPosition(olc::vf2d(-(_EngineContext->ScreenWidth()/2), _EngineContext->ScreenHeight()/2));
        }
        else if (i == 2) {
            sp->SetPosition(olc::vf2d(_EngineContext->ScreenWidth()/2, -(_EngineContext->ScreenHeight()/2)));
        }
        else if (i == 3) {
            sp->SetPosition(olc::vf2d(-(_EngineContext->ScreenWidth()/2), -(_EngineContext->ScreenHeight() / 2)));
        }

        sp->SetCollisionManager(&_CollisionManager);

        // store spawner
        _Spawners.push_back(std::move(sp));

        // prepare atomic flag default false (store via unique_ptr wrapper)
        _SpawnRequested[i] = std::make_unique<std::atomic<bool>>(false);

        // create a timer per spawner (stagger intervals slightly)
        auto spawnTask = [this, i]() {
            if (_SpawnRequested[i]) _SpawnRequested[i]->store(true);
            };
        auto interval = std::chrono::seconds(1 + random_num); // slightly different interval
        _SpawnerTimers.push_back(std::make_unique<PeriodicTimer>(interval, spawnTask));
        _SpawnerTimers.back()->start();
    }
}

void GameManager::StartGameLogic(float elapsedTime) {
    if (!_IsGameStarted) {
		_SpawnersPaused = true; // ensure spawners are paused on start screen
        _EngineContext->Clear(mainUIOrange);

		if (_OptionsSelected) {
            _EngineContext->Clear(bgColorNavyBlue);

            //Exit Button
            DrawButton(olc::vi2d(850, 700), olc::vi2d(200, 41), alertUIYellow);
            _EngineContext->DrawString(855, 715, "EXIT OPTIONS", bgColorNavyBlue, 2);

			if (ButtonDetection(olc::vi2d(850, 700), olc::vi2d(200, 41))) {
				_OptionsSelected = false;
				std::cout << "Exit Options" << std::endl;
			}

            //EASY BUTTON
            DrawButton(olc::vi2d(180, 330), olc::vi2d(300, 41), alertUIYellow);
            _EngineContext->DrawString(200, 350, "Difficulty EASY", bgColorNavyBlue, 2);
            if (ButtonDetection(olc::vi2d(180, 330), olc::vi2d(300, 41))) {
				_DifficultyLevel = EDifficultyLevel::Easy;
            }

			//MEDIUM BUTTON
            DrawButton(olc::vi2d(780, 330), olc::vi2d(300, 41), alertUIYellow);
            _EngineContext->DrawString(800, 350, "Difficulty MEDIUM", bgColorNavyBlue, 2);
            if (ButtonDetection(olc::vi2d(780, 330), olc::vi2d(300, 41))) {
				_DifficultyLevel = EDifficultyLevel::Medium;
            }

			//HARD BUTTON
            DrawButton(olc::vi2d(1380, 330), olc::vi2d(300, 41), alertUIYellow);
            _EngineContext->DrawString(1400, 350, "Difficulty HARD", bgColorNavyBlue, 2);
            if (ButtonDetection(olc::vi2d(1380, 330), olc::vi2d(300, 41))) {
				_DifficultyLevel = EDifficultyLevel::Hard;
            }
        }
		else if (!_OptionsSelected) {
            // Options button
            _EngineContext->FillRect(900, 200, 120, 41, bgColorNavyBlue);
            _EngineContext->FillCircle(895, 220, 20, bgColorNavyBlue);
            _EngineContext->FillCircle(1024, 220, 20, bgColorNavyBlue);
            _EngineContext->DrawString(905, 215, "OPTIONS", alertUIYellow, 2);
            _OptionsSelected = ButtonDetection(olc::vi2d(875, 200), olc::vi2d(149, 41));

            _EngineContext->DrawString(850, 540, "START GAME", bgColorNavyBlue, 3);
            _EngineContext->DrawString(800, 600, "Press SPACE to start", bgColorNavyBlue, 2);

            // Wait for restart input each frame, allow fade-out when restarting
            if (_EngineContext->GetKey(olc::Key::SPACE).bPressed) {
                // start the game once
                StartGame();
                _IsGameStarted = true;
            }
		}
    }
}

void GameManager::EndGameLogic(float elapsedTime) {
    // Do not reset _IsGameStarted here - keep start state until explicit restart
    // Only increase score during active gameplay (not on start screen or when game over)
    if (_IsGameStarted && !_IsGameOver && _Player.GetHealth() > 0) {
        // Increase score over time, faster if player is doing well
        float scoreIncrement = elapsedTime * 10.0f; // base increment
        AddScore(scoreIncrement);
    }

    // Non-blocking game over handling with fade transition and spawner pause
    if (!_IsGameOver && _Player.GetHealth() <= 0) {
        _IsGameOver = true;
        _SpawnersPaused = true; // pause spawner processing
        _IsFadingIn = true;
        _GameOverFade = 0.0f;
    }

    if (_IsGameOver) {
        // advance fade in
        if (_IsFadingIn) {
            _GameOverFade += elapsedTime / _GameOverFadeDuration;
            if (_GameOverFade >= 1.0f) {
                _GameOverFade = 1.0f;
                _IsFadingIn = false;
            }
        }

        // draw overlay with alpha based on fade
        // olc::Pixel takes RGB only; simulate alpha by interpolating color toward DARK_RED
		// Taken from Github Copilot suggestion, looks good and is simple enough to understand, nice job Copilot :D
        auto blend = [&](const olc::Pixel& a, const olc::Pixel& b, float t) {
            uint8_t r = static_cast<uint8_t>(a.r * (1.0f - t) + b.r * t);
            uint8_t g = static_cast<uint8_t>(a.g * (1.0f - t) + b.g * t);
            uint8_t bl = static_cast<uint8_t>(a.b * (1.0f - t) + b.b * t);
            return olc::Pixel(r, g, bl);
        };

        olc::Pixel bg = blend(olc::BLACK, olc::DARK_RED, _GameOverFade);
        _EngineContext->Clear(bg);
        _EngineContext->DrawString(850, 540, "GAME OVER", alertUIYellow, 3);
        _EngineContext->DrawString(800, 600, "Press R to Restart", alertUIYellow, 2);

        // Wait for restart input each frame, allow fade-out when restarting
        if (_EngineContext->GetKey(olc::Key::R).bPressed) {
            // start fade out
            _IsFadingOut = true;
            _IsFadingIn = false;
        }

        if (_IsFadingOut) {
            _GameOverFade -= elapsedTime / _GameOverFadeDuration;
            if (_GameOverFade <= 0.0f) {
                _GameOverFade = 0.0f;
                _IsFadingOut = false;
                // finish game-over state and return to start screen
                // Reset transient gameplay state so EndGameLogic won't immediately re-enter GAME OVER
                StartGame(); // clears bullets/enemies and restores player health
                // but keep spawners paused while on the start screen
                _SpawnersPaused = true;
                _IsGameOver = false;
                // ensure start screen is active (wait for SPACE to start)
                _IsGameStarted = false;
            }
        }
    }
}

void GameManager::StartGame() {
    SetupSpawner();
    _EngineContext->Clear(bgColorNavyBlue);
    // Reset player
	_Player.SetPosition(olc::vf2d(0.0f, 0.0f));
    _Player.SetHealth(100.0f);
    SetScore(0.0f);

    // Clear bullets
    auto& bullets = _Player.GetBullets();
    bullets.clear();

    // Clear enemies in each spawner (protected by mutex)
    for (auto& sp : _Spawners) {
        std::lock_guard<std::mutex> lk(sp->GetEnnemiesMutex());
        auto& enemys = sp->GetEnnemies();
        enemys.clear();
    }

    // Reset spawn requests
    for (auto& req : _SpawnRequested) {
        if (req) req->store(false);
    }

    // Resume spawners and timers
    _SpawnersPaused = false;
    _IsGameOver = false;
}

// Allow player to shoot the button to select it
// Check bullets for collision with the button rectangle (in screen pixels)
const bool GameManager::ButtonDetection(const olc::vi2d& buttonPos, const olc::vi2d& buttonSize) {
    auto& bullets = _Player.GetBullets();
    for (auto& b : bullets) {
        // bullets are in world-space, convert to screen pixels
        olc::vi2d bPixel = _EngineContext->ConvertWorldPositionToPixels(b.GetPosition());
        if (bPixel.x >= buttonPos.x && bPixel.x <= buttonPos.x + buttonSize.x &&
            bPixel.y >= buttonPos.y && bPixel.y <= buttonPos.y + buttonSize.y) {
            // bullet hit the button: mark for removal and return immediately
            b.markedForRemoval = true;
            return true;
        }
    }
    return false;
}

//- Draw button with curved edges by drawing a rectangle and two circles at the ends, all in the same color
//- Take the Rectangle position and size as parameters, as well as the color to draw the button with the edges
void GameManager::DrawButton(olc::vi2d buttonPos, olc::vi2d buttonSize, olc::Pixel color) {
    _EngineContext->FillRect(buttonPos.x, buttonPos.y, buttonSize.x, buttonSize.y, color);
    _EngineContext->FillCircle(buttonPos.x - 5, buttonPos.y + (buttonSize.y/2), (buttonSize.y / 2), color);
    _EngineContext->FillCircle(buttonPos.x + buttonSize.x + 4, buttonPos.y + (buttonSize.y / 2), (buttonSize.y / 2), color);
}
