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

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(1, 20);
    int random_num = distrib(gen);

#ifdef ACADEMIA_EXAMPLE
    _Player.SetPosition(olc::vf2d(0.0f, 0.0f));
    // Setup collision manager references
    _CollisionManager.SetPlayer(&_Player);

    // Initialize player collider via Player API
    _Player.InitializeCollision(&_CollisionManager);

    // Create multiple spawners
    const int spawnerCount = 2; // configurable number of spawners
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
            sp->SetPosition(olc::vf2d(engineContext->ScreenWidth(), engineContext->ScreenHeight()));
        } else if (i == 1) {
            sp->SetPosition(olc::vf2d(-(engineContext->ScreenWidth()), engineContext->ScreenHeight()));
        } else {
            sp->SetPosition(olc::vf2d(100.0f + i * 80.0f, 100.0f));
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
        auto interval = std::chrono::seconds(5 + random_num); // slightly different interval
        _SpawnerTimers.push_back(std::make_unique<PeriodicTimer>(interval, spawnTask));
        _SpawnerTimers.back()->start();
    }

    // CollisionManager can still infer ennemies from registered colliders, so we don't need to pass containers explicitly
    _CollisionManager.SetBullets(&_Player.GetBullets());
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

    _CollisionManager.Update();

    /* SPAWNER handled by PeriodicTimer started in Initialize() */
    // Process spawn requests signalled by timers for each spawner
    for (size_t i = 0; i < _Spawners.size(); ++i) {
        if (_SpawnRequested[i] && _SpawnRequested[i]->exchange(false)) {
            // spawn on main thread to keep CollisionManager usage single-threaded
            _Spawners[i]->SpawnEnnemies(*_EngineContext, &_Player, &_CollisionManager);
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
    }

    _Player.AddForce(*_EngineContext, 180.0f, direction, elapsedTime);
    _Player.DrawCursor(*_EngineContext, _Player.GetCursorPosition(*_EngineContext));
    _Player.Update(*_EngineContext, elapsedTime);
    _Player.Draw(*_EngineContext);

    // CollisionManager continues to infer enemies via registered colliders; update bullets binding
    _CollisionManager.SetBullets(&_Player.GetBullets());
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