#include "GameManager.h"
#include "../../external/olc/olcPixelGameEngine.h"
#include "../Engine/AcademiaEngine.h" 
#include <chrono>
#include <memory>
#include <mutex>
#include <random>
#include "LightEnemy.h"
#include "Cthulhu.h"


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
        InitializeGame();
    }
    catch (...) { _Success = false; }
    return _Success;
#endif
}

void GameManager::Update(float elapsedTime)
{
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

    for (auto& spawner : _Spawners)
    {
		// Update each spawner only if the spawner is not paused and the game state is playing
		if (_GameState == EGameState::Playing && !_SpawnersPaused)
        spawner->Update(*_EngineContext,
            elapsedTime);
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
        for (auto& enemyPtr : _Enemies)
        {
            auto& enemy = *enemyPtr;

            enemy.SetGameManager(this);
            enemy.Update(*_EngineContext, elapsedTime);
            enemy.Draw(*_EngineContext);

            if (enemy.GetHealth() <= 0.0f && !enemy.hasExploded)
            {
                _Explosions.emplace_back(enemy.GetPosition(),
                    ExplosionType::Enemy);
            }
        }

        Ennemies::RemoveEnnemie(_Enemies, *_EngineContext);

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

		//First boss fight is Chtulhu, so we can check if the score is above 50k to start the fight.
        if (_Score >= 1000.0f && !inBossFight) {
			//Stop the spawners and remove all the ennemies on the screen to start the boss fight.
            StartChtulhuFight();
			_Cthulhu->SetPlayer(&_Player);
        }
        if (_Cthulhu) {
			_Cthulhu->Update(*_EngineContext, elapsedTime);
			_Cthulhu->Draw(*_EngineContext);
            if (_Cthulhu->IsDialogueActive()) _Cthulhu->DrawDialogue(*_EngineContext);
        }

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
bool GameManager::Uninitialize()
{
    _Player.ShutdownCollision(&_CollisionManager);
    return true;
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
        _EngineContext->DrawString(830, 12, "Difficulty : " + GetDifficultyLevelString(), alertUIYellow, 2);
}

//Setup the spawner of ennemies.
//➥ Randomness of spawn time of each individual ennemie and spawner
//➥ Pointer of each spawner
//➥ Position of each spawner
//➥ Collision of ennemies spawned by each spawner
bool GameManager::SetupSpawner()
{
    _Success = true;

    int spawnerCount = 2 + static_cast<int>(_DifficultyLevel);

    _Spawners.clear();
    _Spawners.reserve(spawnerCount);

    for (int i = 0; i < spawnerCount; ++i)
    {
        auto sp = std::make_unique<Spawner>();

        if (!sp)
        {
            _Success = false;
            continue;
        }

        sp->SetGameManager(this);

        _Spawners.push_back(std::move(sp));
    }

    return _Success;
}

//Main function to spawn a random enemy at a random position around the player, outside the screen bounds.
void GameManager::SpawnRandomEnemy()
{
    static std::mt19937 rng(std::random_device{}());

    const olc::vf2d playerPos = _Player.GetPosition();

    std::uniform_real_distribution<float> angleDist(
        0.0f,
        2.0f * 3.14159265f
    );

    float angle = angleDist(rng);

    float spawnDistance =
        std::max(
            _EngineContext->ScreenWidth(),
            _EngineContext->ScreenHeight()
        ) + 50.0f;

    olc::vf2d spawnPos =
    {
        playerPos.x + cosf(angle) * spawnDistance,
        playerPos.y + sinf(angle) * spawnDistance
    };

    std::uniform_int_distribution<int> enemyTypeDist(0, 1);

    EEnemyType type =
        static_cast<EEnemyType>(
            enemyTypeDist(rng));

    SpawnEnemy(type, spawnPos);
}

//Spawn an enemy of the specified type at the given position, and add it to the list of active enemies.
void GameManager::SpawnEnemy(EEnemyType type, const olc::vf2d& position)
{
    std::unique_ptr<Ennemies> enemy;

	// Enemy type selection based on random value
    switch (type)
    {
        case EEnemyType::Fast:
        enemy = std::make_unique<LightEnemy>(position, 12.0f, 20.0f);
        break;

    case EEnemyType::Basic:
        enemy = std::make_unique<Ennemies>(position, 20.0f, 50.0f);
        break;

    case EEnemyType::Tank:
        //enemy = std::make_unique<FastEnemy>(position);
        break;
    }

    if (!enemy) return;

    enemy->SetPlayer(&_Player);
    enemy->SetGameManager(this);
    enemy->InitializeCollision(&_CollisionManager);
	enemy->SetPosition(position);

    _Enemies.push_back(std::move(enemy));
}

void GameManager::EndGameLogic(float elapsedTime) {
    // Only increase score during active gameplay (not on start screen or when game over)
    if (_GameState == EGameState::Playing && _Player.GetHealth() > 0) {
        // Increase score over time
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
                InitializeGame(); // clears bullets/enemies and restores player health
                _SpawnersPaused = true;
				_EngineContext->SetState(EEngineState::Lobby);
            }
        }
    }
}

void GameManager::InitializeGame() {
    _SpawnersPaused = false;
    _Spawners.clear();

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

    _Enemies.clear();
}

//➥ Start the Chtulhu fight by spawning the Chtulhu enemy and setting up any necessary game state for the fight
void GameManager::StartChtulhuFight() {
    inBossFight = true;
    _SpawnersPaused = true; 
    _Enemies.clear();
    _PowerUps.clear();

	// Spawn Chtulhu enemy outside the top of the screen, centered horizontally and comming to the top of the screen to be visible
    _Cthulhu = std::make_unique<Cthulhu>(olc::vf2d(_EngineContext->ScreenWidth()/2,
        _EngineContext->ScreenHeight() + 25));
}
