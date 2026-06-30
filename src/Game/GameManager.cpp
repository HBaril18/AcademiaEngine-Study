#include "GameManager.h"
#include "../../external/olc/olcPixelGameEngine.h"
#include "../Engine/AcademiaEngine.h" 
#include <chrono>
#include <memory>
#include <mutex>
#include <random>


GameManager::GameManager(AcademiaEngine* engine)
    : _Player(*engine)
{
    _EngineContext = engine;
}


bool GameManager::Initialize(AcademiaEngine* engineContext)
{
    _Success = true;
    if (!_EngineContext) _Success = false;

#ifdef ACADEMIA_EXAMPLE
    try {
        _Player.SetGameManager(this);
        _Player.GetSprite(*_EngineContext);
        _Player.SetPosition(olc::vf2d(0.0f, 0.0f));
        // Setup collision manager references
        _CollisionManager.SetPlayer(&_Player);
        // Initialize player collider via Player API
        _Player.InitializeCollision(&_CollisionManager);
    }
    catch (...) { _Success = false; }
    return _Success;
#endif
}

void GameManager::Update(float elapsedTime)
{
    StartGameLogic(elapsedTime);

    
    //Gameplay code
    //Movement handle
    float x = 0.0f;
    float y = 0.0f;

    constexpr olc::Key rightKey = olc::Key::D;
    constexpr olc::Key leftKey = olc::Key::A;
    constexpr olc::Key upKey = olc::Key::W;
    constexpr olc::Key downKey = olc::Key::S;
    constexpr olc::Key spaceKey = olc::Key::SPACE;
    constexpr olc::Key shiftKey = olc::Key::SHIFT;
    constexpr olc::Key escapeKey = olc::Key::ESCAPE;
    const olc::HWButton moveRightButton = _EngineContext->GetKey(rightKey);
    const olc::HWButton moveLeftButton = _EngineContext->GetKey(leftKey);
    const olc::HWButton moveUpButton = _EngineContext->GetKey(upKey);
    const olc::HWButton moveDownButton = _EngineContext->GetKey(downKey);
    const olc::HWButton jumpButton = _EngineContext->GetKey(spaceKey);
    const olc::HWButton sneakButton = _EngineContext->GetKey(shiftKey);
    const olc::HWButton escapeButton = _EngineContext->GetKey(escapeKey);

    // This is very good, I don't like input code, it looks ugly I find :p, but there is no better way than this. Good Job :) 
    if (escapeButton.bPressed) { 
        StartExitAnimation();

    }
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
    if (_GameState == EGameState::Playing) {
        // iterate enemies for each spawner (each spawner protects its own container)
        for (auto& sp : _Spawners)
        {
            std::lock_guard<std::mutex> lk(sp->GetEnnemiesMutex());
            auto& enemys = sp->GetEnnemies();

            for (auto& enemyPtr : enemys)
            {
                auto& enemy = *enemyPtr;
                enemy.SetGameManager(this);
                enemy.Update(*_EngineContext, elapsedTime);
                enemy.Draw(*_EngineContext);

                if (enemy.GetHealth() <= 0.0f && !enemy.hasExploded)
                {
                    _Explosions.emplace_back(enemy.GetPosition(), ExplosionType::Enemy);
                }
            }
            Ennemies::RemoveEnnemie(enemys, *_EngineContext);
            _Player.AddScore(10.0f * enemys.size() * elapsedTime);
        }

        for (auto& p : _PowerUps)
        {
            p->SetGameManager(this);
            p->Update(*_EngineContext, elapsedTime);
            p->Draw(*_EngineContext);
        }

        _PowerUps.erase(
            std::remove_if(
                _PowerUps.begin(),
                _PowerUps.end(),
                [](const std::unique_ptr<PowerUp>& p)
                {
                    return p->markedForRemoval;
                }),
            _PowerUps.end());

        for (auto& n : _PowerUpNotifications)
        {
            n.timer -= elapsedTime;
        }

        _PowerUpNotifications.erase(
            std::remove_if(
                _PowerUpNotifications.begin(),
                _PowerUpNotifications.end(),
                [](const PowerUpNotification& n)
                {
                    return n.timer <= 0.0f;
                }),
            _PowerUpNotifications.end());

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
        _CollisionManager.Update(elapsedTime);

        // collision logic handled here but bullets already cleaned up
    } else {
        // On start screen, still draw cursor so player can aim and shoot buttons
        _Player.DrawCursor(*_EngineContext, _Player.GetCursorPosition(*_EngineContext));
    }

    //EXPLOSION ANIMATION
    for (auto& e : _Explosions)
    {
        e.Update(elapsedTime);
        e.Draw(_EngineContext);
    }

    _Explosions.erase(
        std::remove_if(_Explosions.begin(), _Explosions.end(),
            [](const Explosion& e) {
                return e.finished;
            }),
        _Explosions.end());

    // UI code
    // Draw Player health bar
    _Player.GetPosition();
    olc::vi2d healthBarPos = _EngineContext->ConvertWorldPositionToPixels(_Player.GetPosition()) + olc::vi2d(-20, -30);
    if (_GameState == EGameState::Playing) _EngineContext->FillRect(healthBarPos, olc::vi2d(40, 5), olc::WHITE);
    if (_Player.GetHealth() > 0 && _GameState == EGameState::Playing) {
        int healthWidth = static_cast<int>(40 * (_Player.GetHealth() / 100.0f));
        _EngineContext->FillRect(healthBarPos, olc::vi2d(healthWidth, 5), olc::GREEN);
    }
    DrawUI();
    DrawPowerUpUI();
    _EngineContext->DrawString(10, 12, "FPS : " + std::to_string(_EngineContext->GetFPS()), alertUIYellow, 2);
    EndGameLogic(elapsedTime);
#endif
}

void GameManager::StartExitAnimation()
{
    _FallingPixels.clear();

    for (int y = 0; y < _EngineContext->ScreenHeight(); y++)
    {
        for (int x = 0; x < _EngineContext->ScreenWidth(); x++)
        {
            FallingPixel p;
            p.pos = { x, y };
            p.color = _EngineContext->GetDrawTarget()->GetPixel(x, y);
            p.velocity = 0.0f;

            _FallingPixels.push_back(p);
        }
    }
}

//Clear all the pointer after the game close to clear the memory.
//➥Ex: _Spawner, _Player, etc.
bool GameManager::Uninitialize() {
    _Success = true;
    for (auto& t : _SpawnerTimers) {
        if (t) {
            t->stop();
            _Success = true;
        }
        else _Success = false;
    }
    try {
        _SpawnerTimers.clear();

        _Player.ShutdownCollision(&_CollisionManager);
    }
    catch (...) { _Success = false; }
    return _Success;
}

//Destructor of GameManager to Unitialize pointer.
GameManager::~GameManager()
{
	Uninitialize();
}

void GameManager::AddPowerUpNotification(const std::string& text)
{
    _PowerUpNotifications.push_back({
        text,
        2.0f,
        2.0f
        });
}

//Draw the UI of the PowerUp just collected
//➥ Heal
//➥ Speed
//➥ Damage
//➥ Shield
void GameManager::DrawPowerUpUI()
{
    int y = 400;

    for (auto& n : _PowerUpNotifications)
    {
        float alpha = n.timer / n.maxTimer;

        olc::Pixel color = alertUIYellow;
        color.a = static_cast<uint8_t>(255.0f * alpha);

        _EngineContext->DrawString(
            _EngineContext->ScreenWidth() / 2,
            y,
            n.text,
            color,
            2
        );

        y += 24;
    }
}

//Draw the Main UI of the game.
//➥ Top main bar
//➥ FPS
//➥ Difficulty
//➥ Current Score
void GameManager::DrawUI() {
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
    default:
        break;
    }
}

//Setup the spawner of ennemies.
//➥ Randomness of spawn time of each individual ennemie and spawner
//➥ Pointer of each spawner
//➥ Position of each spawner
//➥ Collision of ennemies spawned by each spawner
bool GameManager::SetupSpawner() {
    _Success = true;
    std::random_device rd;
    std::mt19937 gen(rd());

    int spawnerCount = 2 + static_cast<int>(_DifficultyLevel);

    _Spawners.reserve(spawnerCount);
    _SpawnerTimers.reserve(spawnerCount);
    _SpawnRequested.resize(spawnerCount);

    for (int i = 0; i < spawnerCount; ++i) {

        auto sp = std::make_unique<Spawner>();
        if (!sp) _Success = false;

        if (i == 0)
            sp->SetPosition(olc::vf2d(_EngineContext->ScreenWidth() / 2, _EngineContext->ScreenHeight() / 2 ));
        else if (i == 1)
            sp->SetPosition(olc::vf2d(-_EngineContext->ScreenWidth() / 2, _EngineContext->ScreenHeight() / 2 ));
        else if (i == 2)
            sp->SetPosition(olc::vf2d(_EngineContext->ScreenWidth() / 2, -_EngineContext->ScreenHeight() / 2 ));
        else if (i == 3)
            sp->SetPosition(olc::vf2d(-_EngineContext->ScreenWidth() / 2, -_EngineContext->ScreenHeight() / 2 ));

        sp->SetCollisionManager(&_CollisionManager);
        _Spawners.push_back(std::move(sp));

        _SpawnRequested[i] = std::make_unique<std::atomic<bool>>(false);
        if (!_SpawnRequested[i]) _Success = false;


        /*=======THIS PORTION OF CODE WAS MADE WITH THE HELP OF COPILOT========*/
        // 1. interval initial
        std::uniform_int_distribution<int> distrib(3, 10);
        auto interval = std::chrono::seconds(distrib(gen));

        // 2. créer le timer d'abord (sans task)
        auto timer = std::make_unique<PeriodicTimer>(interval, []() {});
        if (!timer) _Success = false;
        
        auto timerPtr = timer.get();
        if (!timerPtr) _Success = false;

        // 3. créer la task avec pointeurs stables
        auto spawnTask = [this, i, timerPtr]() {

            if (_SpawnRequested[i])
                _SpawnRequested[i]->store(true);

            static thread_local std::mt19937 gen(std::random_device{}());
            std::uniform_int_distribution<int> distrib(3, 10);

            int nextDelay = distrib(gen);
            };

        // 4. injecter la task
        timer->setTask(spawnTask);

        // 5. stocker
        _SpawnerTimers.push_back(std::move(timer));

        // 6. start
        try { _SpawnerTimers.back()->start(); }
        catch (...) {
            _Success = false;
        }
        /*==================================================================*/
    }
    return _Success;
}

//Options UI and logic for it.
//➥ EXIT OPTIONS button
//➥ EASY button
//➥ MEDIUM button
//➥ HARD button
//➥ Redrawing of OPTIONS button and "Press SPACE to start" text
void GameManager::StartGameLogic(float elapsedTime) {
    if (_GameState == EGameState::Menu) {
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
                _GameState = EGameState::Playing;
                // start the game once
                StartGame();
                
            }
		}
    }
}

void GameManager::EndGameLogic(float elapsedTime) {
    // Do not reset _IsGameStarted here - keep start state until explicit restart
    // Only increase score during active gameplay (not on start screen or when game over)
    if (_GameState == EGameState::Playing && _Player.GetHealth() > 0) {
        // Increase score over time, faster if player is doing well
        float scoreIncrement = elapsedTime * 5.0f; // base increment
        AddScore(scoreIncrement);
    }

    // Non-blocking game over handling with fade transition and spawner pause
    if (_GameState != EGameState::GameOver && _Player.GetHealth() <= 0) {
        _GameState = EGameState::GameOver;
        _SpawnersPaused = true;

        if (!_Player.hasExploded) {
            _Explosions.emplace_back(_Player.GetPosition(), ExplosionType::Player);
            _Player.hasExploded = true;
        }

        _GameOverTimer = 0.0f;
    }


    if (_GameState == EGameState::GameOver)
    {
        bool explosionStillRunning = false;

        for (auto& e : _Explosions)
        {
            if (!e.finished)
            {
                explosionStillRunning = true;
                break;
            }
        }

        if (!explosionStillRunning)
        {
            _GameState = EGameState::GameOver;
            _IsFadingIn = true;
            _GameOverFade = 0.0f;
        }
    }



    if (_GameState == EGameState::GameOver) {
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
                _GameState = EGameState::Menu;
                StartGame(); // clears bullets/enemies and restores player health
                // but keep spawners paused while on the start screen
                _SpawnersPaused = true;
            }
        }
    }
}

void GameManager::StartGame() {
    //Clear all spawner
    for (auto& t : _SpawnerTimers) {
        if (t) t->stop();
    }
    _Spawners.clear();
    _SpawnerTimers.clear();
    _SpawnRequested.clear();

    SetupSpawner();

    _EngineContext->Clear(bgColorNavyBlue);

    _Player.hasExploded = false;
    _GameOverTimer = 0.0f;

    // Clear explosions (important)
    _Explosions.clear();

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
}

//➥ Allow player to shoot the button to select it
//➥ Check bullets for collision with the button rectangle (in screen pixels)
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

//➥ Draw button with curved edges by drawing a rectangle and two circles at the ends, all in the same color
//➥ Take the Rectangle position and size as parameters, as well as the color to draw the button with the edges
void GameManager::DrawButton(olc::vi2d buttonPos, olc::vi2d buttonSize, olc::Pixel color) {
    _EngineContext->FillRect(buttonPos.x, buttonPos.y, buttonSize.x, buttonSize.y, color);
    _EngineContext->FillCircle(buttonPos.x - 5, buttonPos.y + (buttonSize.y/2), (buttonSize.y / 2), color);
    _EngineContext->FillCircle(buttonPos.x + buttonSize.x + 4, buttonPos.y + (buttonSize.y / 2), (buttonSize.y / 2), color);
}