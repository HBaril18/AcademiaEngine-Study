#include "GameManager.h"
#include "../../external/olc/olcPixelGameEngine.h"
#include "../Engine/AcademiaEngine.h"
#include <chrono>

void GameManager::Initialize(AcademiaEngine* engineContext)
{
    _EngineContext = engineContext;

#ifdef ACADEMIA_EXAMPLE
    _Player.SetPosition(olc::vf2d(0.0f, 0.0f));
    _Spawner.SetPosition(olc::vf2d(100.0f, 100.0f));

    // Setup spawner timer to run every 5 seconds
    auto spawnTask = [this]() {
        _SpawnRequested.store(true);
    };

    _SpawnerTimer = std::make_unique<PeriodicTimer>(std::chrono::seconds(5), spawnTask);
    _SpawnerTimer->start();
#endif
}

void GameManager::Update(float elapsedTime)
{
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
    
    // Gameplay code
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

    // Process spawn requests signalled by the timer
    if (_SpawnRequested.exchange(false)) {
        _Spawner.SpawnEnnemies(*_EngineContext, &_Player, &_CollisionManager);
    }

    // Normalize diagonal movement
    if (x != 0.0f && y != 0.0f) {
        x *= 0.5f;
        y *= 0.5f;
    }

    std::vector<float> direction = { x, y };

#ifdef ACADEMIA_EXAMPLE
	std::deque<Bullet>& bullets = _Player.GetBullets();
    for (auto& bullet : bullets) {
        bullet.Update(*_EngineContext, elapsedTime);
        bullet.Draw(*_EngineContext);
    }

    std::deque<Ennemies>& enemys = _Spawner.GetEnnemies();
    for (auto& enemy : enemys)
    {
        enemy.Update(*_EngineContext, elapsedTime);
        enemy.Draw(*_EngineContext);
    }
    _Ennemies.RemoveEnnemie(enemys);
    
    _Player.AddForce(*_EngineContext, 180.0f, direction, elapsedTime);
    _Player.DrawCursor(*_EngineContext, _Player.GetCursorPosition(*_EngineContext));
    _Player.Update(*_EngineContext, elapsedTime);
    _Player.Draw(*_EngineContext);
#endif
}

void GameManager::Uninitialize() {
    //detruit les new et pointeur que j'ai cr�er
    if (_SpawnerTimer) {
        _SpawnerTimer->stop();
        _SpawnerTimer.reset();
    }
}