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

    /*CONTROLS*/
    if (escapeButton.bPressed) {
        StartExitAnimation();

    }
    if (moveRightButton.bHeld) { x += 1.0f; }
    if (moveLeftButton.bHeld) { x -= 1.0f; }
    if (moveUpButton.bHeld) { y += 1.0f; }
    if (moveDownButton.bHeld) { y -= 1.0f; }
    if (sneakButton.bHeld) { x *= 0.2f; y *= 0.2f; }
    if (_EngineContext->GetMouse(0).bPressed) {
        _Player.SpawnBullet(*_EngineContext);
    }

    // Normalize diagonal movement
    if (x != 0.0f && y != 0.0f) {
        x *= 0.5f;
        y *= 0.5f;
    }

    std::vector<float> direction = { x, y };

    if (_GameState == EGameState::Playing) {
        _Player.AddForce(*_EngineContext, 180.0f, direction, elapsedTime);
    }

    //FIRST ENTRY IN GAMEOVER
    if (_GameState != EGameState::GameOver && _Player.GetHealth() <= 0) {
        _GameState = EGameState::GameOver;
        _SpawnersPaused = true;

        if (!_Player.hasExploded) {
            _Explosions.emplace_back(_Player.GetPosition(), ExplosionType::Player);
            _Player.hasExploded = true;
        }

        _GameOverTimer = 0.0f;
    }

    //ACTUAL GAMEOVER
    if (_GameState == EGameState::GameOver)
    {
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

        EndGameLogic(elapsedTime);
        return;
    }

    //PLAYING STATE
    if (_GameState == EGameState::Playing) {
        /*1. SPAWNER UPDATE*/
        for (auto& spawner : _Spawners)
        {
            // Update each spawner only if the spawner is not paused and the game state is playing
            if (!_SpawnersPaused)
                spawner->Update(*_EngineContext,
                    elapsedTime);
        }

        /*2. PLAYER UPDATE*/
        _Player.Update(*_EngineContext, elapsedTime);

        // 3. ENEMY UPDATE
        for (auto& enemyPtr : _Enemies)
        {
            enemyPtr->Update(*_EngineContext, elapsedTime);

            if (enemyPtr->GetHealth() <= 0.0f &&
                !enemyPtr->hasExploded)
            {
                _Explosions.emplace_back(
                    enemyPtr->GetPosition(),
                    ExplosionType::Enemy
                );

                enemyPtr->hasExploded = true;
            }
        }

        // 3.1 BOSS UPDATE
        if (_Cthulhu)
        {
            _Cthulhu->Update(*_EngineContext, elapsedTime);
        }

        // 3.2 BOSS BULLET UPDATE
        for (auto& eBullet : _EnemyBullets)
        {
            eBullet.Update(*_EngineContext, elapsedTime);
        }

        // 4. BULLET UPDATE
        for (auto& bullet : _Player.GetBullets())
        {
            bullet.Update(*_EngineContext, elapsedTime);
        }

        // 5. COLLISION UPDATE
        _CollisionManager.Update(elapsedTime);

        // 6. CLEAN BULLETS
        _Player.UpdateBullets(*_EngineContext);

        // 7. ENEMY BULLET REMOVAL
        for (auto it = _EnemyBullets.begin(); it != _EnemyBullets.end(); )
        {
            EnemyBullet& bullet = *it;

            bool remove = bullet.markedForRemoval;

            olc::vi2d pixel =
                _EngineContext->ConvertWorldPositionToPixels(
                    bullet.GetPosition());

            bool out =
                pixel.x < 0 ||
                pixel.x > _EngineContext->ScreenWidth() ||
                pixel.y < 0 ||
                pixel.y > _EngineContext->ScreenHeight();

            if (remove || out)
            {
                bullet.ShutdownCollision(&_CollisionManager);
                it = _EnemyBullets.erase(it);
            }
            else
            {
                ++it;
            }
        }

        //ENEMY BULLET DRAW
        for (auto& bullet : _EnemyBullets)
        {
            bullet.Draw(*_EngineContext);
        }

        // BULLET DRAW
        for (auto& bullet : _Player.GetBullets())
        {
            bullet.Draw(*_EngineContext);
        }

        // ENEMY DRAW
        for (auto& enemyPtr : _Enemies)
        {
            enemyPtr->Draw(*_EngineContext);
        }

        // BOSS DRAW
        if (_Cthulhu)
        {
            _Cthulhu->Draw(*_EngineContext);

            if (_Cthulhu->IsDialogueActive())
            {
                if (_Cthulhu->GetState() == BossState::Dying)
                {
                    _Cthulhu->DrawDialogue(
                        *_EngineContext,
                        _Cthulhu->GetDeathDialogue());
                }
                else
                {
                    _Cthulhu->DrawDialogue(
                        *_EngineContext,
                        _Cthulhu->GetIntroDialogue());
                }
            }
        }

        // PLAYER DRAW
        _Player.DrawCursor(*_EngineContext, _Player.GetCursorPosition(*_EngineContext));
        _Player.Draw(*_EngineContext);
    }

#ifdef ACADEMIA_EXAMPLE
    if (_GameState == EGameState::Playing) {
        Ennemies::RemoveEnnemie(_Enemies, *_EngineContext);

        for (auto& p : _PowerUps)
        {
            //p->SetGameManager(this);
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

		//First boss fight is Chtulhu, so we can check if the score is above 50k to start the fight.
        if (_Score >= 10.0f && !inBossFight) {
			//Stop the spawners and remove all the ennemies on the screen to start the boss fight.
            StartChtulhuFight();
        }
        if (_Cthulhu) {
            if (_Cthulhu->GetState() == BossState::Dead)
            {
                _Explosions.emplace_back(
                    _Cthulhu->GetPosition(),
                    ExplosionType::Enemy);

                _Cthulhu.reset();

                ResumeGame();
            }
        }
    } 
    else {
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
    DrawPlayerHealthBar(_EngineContext);

    DrawUI();
    DrawPowerUpUI();
    _EngineContext->DrawString(10, 12, "FPS : " + std::to_string(_EngineContext->GetFPS()), alertUIYellow, 2);

    // Only increase score during active gameplay (not on start screen or when game over)
    if (_GameState == EGameState::Playing && _Player.GetHealth() > 0) {
        // Increase score over time
        float scoreIncrement = elapsedTime * 5.0f; // base increment
        AddScore(scoreIncrement);
    }
#endif
}

void GameManager::SpawnEnemyBullet(const olc::vf2d& position, const olc::vf2d& target)
{
    _EnemyBullets.emplace_back();

    EnemyBullet& eBullet = _EnemyBullets.back();

    eBullet.SetPosition(position);
    eBullet.SetSprite(&_EngineContext->EnemyBullet);
    if (_DifficultyLevel == EDifficultyLevel::Easy) { eBullet.SetSpeed(700.0f); }
    if (_DifficultyLevel == EDifficultyLevel::Medium) { eBullet.SetSpeed(800.0f); }
    if (_DifficultyLevel == EDifficultyLevel::Hard) { eBullet.SetSpeed(1000.0f); }
    std::cout << "EBullet speed : " << eBullet.speed;
    eBullet.sprite = &_EngineContext->EnemyBullet;
    eBullet.decal = new olc::Decal(eBullet.sprite);

    olc::vf2d dir = target - position;

    if (dir.mag2() > 0.001f)
        dir = dir.norm();
    else
        dir = { 1.0f, 0.0f };

    eBullet.SetDirection(dir);
    eBullet.InitializeCollision(&_CollisionManager);
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

        if (!explosionStillRunning && !_IsFadingIn && _GameOverFade <= 0.0f)
        {
            _IsFadingIn = true;
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
        float t = _GameOverFade;
        float eased = t * t * (3.0f - 2.0f * t); // SmoothStep
        auto blend = [&](const olc::Pixel& a, const olc::Pixel& b, float eased) {
            uint8_t r = static_cast<uint8_t>(a.r * (1.0f - t) + b.r * t);
            uint8_t g = static_cast<uint8_t>(a.g * (1.0f - t) + b.g * t);
            uint8_t bl = static_cast<uint8_t>(a.b * (1.0f - t) + b.b * t);
            return olc::Pixel(r, g, bl);
        };

        olc::Pixel bg =
            blend(olc::BLACK,
                olc::DARK_RED,
                eased);

        _EngineContext->FillRect(
            0,
            0,
            _EngineContext->ScreenWidth(),
            _EngineContext->ScreenHeight(),
            bg
        );
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
				_EngineContext->SetState(EEngineState::Lobby);
            }
        }
    }
}

void GameManager::InitializeGame() {
    
    _GameState = EGameState::Playing;
    _IsFadingIn = false;
    _IsFadingOut = false;
    _GameOverFade = 0.0f;
    _GameOverTimer = 0.0f;
    _Player.hasExploded = false;
    inBossFight = false;
    _Cthulhu.reset();

    _SpawnersPaused = false;
    _Spawners.clear();

    SetupSpawner();

    _EngineContext->Clear(bgColorNavyBlue);

    // Clear explosions (important)
    _Explosions.clear();

    // Reset player
	_Player.SetPosition(olc::vf2d(0.0f, 0.0f));
    _Player.SetHealth(100.0f);
    SetScore(0.0f);

    // Clear bullets
    auto& bullets = _Player.GetBullets();
    bullets.clear();

	ClearEnnemyPtrs();
	ClearPowerUpPtrs();

    for (auto& bullet : _EnemyBullets)
    {
        bullet.ShutdownCollision(&_CollisionManager);
    }

    _EnemyBullets.clear();
}

void GameManager::ClearEnnemyPtrs() {
    for (auto& enemy : _Enemies)
    {
        if (enemy && enemy->collider)
        {
            _CollisionManager.UnregisterCollider(enemy->collider.get());
        }
    }
    _Enemies.clear();
}
void GameManager::ClearPowerUpPtrs() {
    for (auto& powerUp : _PowerUps)
    {
        if (powerUp && powerUp->collider)
        {
            _CollisionManager.UnregisterCollider(powerUp->collider.get());
        }
    }
    _PowerUps.clear();
}

//Start the Chtulhu fight by spawning the Chtulhu enemy and setting up any necessary game state for the fight such as :
//➥ Pausing spawners
//➥ Clearing existing enemies and power-ups
//➥ Setting the inBossFight flag to true
//➥ Positioning Chtulhu outside the top of the screen, centered horizontally, and moving it into view
//➥ Initializing Chtulhu's collision with the collision manager
void GameManager::StartChtulhuFight()
{
    inBossFight = true;
    _SpawnersPaused = true;

	ClearEnnemyPtrs();
	ClearPowerUpPtrs();

    _Cthulhu = std::make_unique<Cthulhu>(
        olc::vf2d(
            _EngineContext->ScreenWidth() / 2.0f,
            _EngineContext->ScreenHeight() + 25.0f
        )
    );
    _Cthulhu->collider->layer = 2;
    _Cthulhu->SetGameManager(this);
    _Cthulhu->SetPlayer(&_Player);
    _Cthulhu->InitializeCollision(&_CollisionManager);
    if (_DifficultyLevel == EDifficultyLevel::Easy) { _Cthulhu->SetMaxHealth(1000.0f); }
    if (_DifficultyLevel == EDifficultyLevel::Medium) { _Cthulhu->SetMaxHealth(1200.0f); }
    if (_DifficultyLevel == EDifficultyLevel::Hard) { _Cthulhu->SetMaxHealth(1500.f); }
}

void GameManager::ResumeGame() {
	_SpawnersPaused = false;
}

void GameManager::DrawPlayerHealthBar(AcademiaEngine* engineContext)
{
    float healthRatio =
        std::clamp(_Player.GetHealth() / 100.0f, 0.0f, 1.0f);

    const olc::vi2d barPos = { 24, 120 };

    constexpr int barWidth = 28;
    constexpr int barHeight = 300;

    // Colors
    olc::Pixel darkPanel = olc::Pixel(12, 4, 24);

    olc::Pixel frameDark = olc::Pixel(45, 10, 80);
    olc::Pixel frameMid = olc::Pixel(110, 35, 180);
    olc::Pixel frameLight = olc::Pixel(235, 190, 255);

    olc::Pixel healthColor = olc::Pixel(170, 45, 255);
    olc::Pixel healthHighlight = olc::Pixel(240, 180, 255);

    // -----------------------------
    // Shadow
    // -----------------------------
    engineContext->FillRect(
        barPos + olc::vi2d(5, 6),
        { barWidth, barHeight },
        olc::Pixel(0, 0, 0)
    );

    // -----------------------------
    // Outer glow frame
    // -----------------------------
    engineContext->DrawRect(
        barPos - olc::vi2d(4, 4),
        { barWidth + 8, barHeight + 8 },
        frameDark
    );

    engineContext->DrawRect(
        barPos - olc::vi2d(2, 2),
        { barWidth + 4, barHeight + 4 },
        frameMid
    );

    engineContext->DrawRect(
        barPos - olc::vi2d(1, 1),
        { barWidth + 2, barHeight + 2 },
        frameLight
    );

    // -----------------------------
    // Inner background
    // -----------------------------
    engineContext->FillRect(
        barPos,
        { barWidth, barHeight },
        darkPanel
    );

    // -----------------------------
    // Health fill
    // -----------------------------
    int fillHeight =
        static_cast<int>(barHeight * healthRatio);

    if (fillHeight > 0)
    {
        int fillY =
            barPos.y + (barHeight - fillHeight);

        engineContext->FillRect(
            { barPos.x, fillY },
            { barWidth, fillHeight },
            healthColor
        );

        // Bright strip
        engineContext->FillRect(
            { barPos.x + 2, fillY },
            { 4, fillHeight },
            healthHighlight
        );

        // Bright top edge
        engineContext->DrawLine(
            { barPos.x + 1, fillY },
            { barPos.x + barWidth - 2, fillY },
            olc::WHITE
        );
    }

    // -----------------------------
    // Segments
    // -----------------------------
    constexpr int segmentCount = 10;

    for (int i = 1; i < segmentCount; i++)
    {
        int y =
            barPos.y +
            (barHeight / segmentCount) * i;

        engineContext->DrawLine(
            { barPos.x + 2, y },
            { barPos.x + barWidth - 3, y },
            olc::Pixel(55, 20, 95)
        );
    }

    // -----------------------------
    // Inner border
    // -----------------------------
    engineContext->DrawRect(
        barPos,
        { barWidth, barHeight },
        frameLight
    );

    // -----------------------------
    // HP text
    // -----------------------------
    engineContext->DrawString(
        barPos.x - 4,
        barPos.y + barHeight + 16,
        std::to_string((int)_Player.GetHealth()),
        olc::WHITE,
        2
    );
}
