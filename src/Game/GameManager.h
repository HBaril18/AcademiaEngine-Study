#pragma once
#include <Utils.h>
#include <memory>
#include <atomic>
#include "../../PeriodicTimer.h"
#include <vector>

#define ACADEMIA_EXAMPLE

#ifdef ACADEMIA_EXAMPLE
#include "ExampleGameObject.h"
#include "Player.h"
#include "Bullet.h"
#include "Ennemies.h"
#include "Spawner.h"
#include "../../CollisionManager.h"
#endif

class AcademiaEngine;

class GameManager
{

public:

    void Initialize(AcademiaEngine* engineContext);
    void Update(float elapsedTime);
    void Uninitialize();

private:

    AcademiaEngine* _EngineContext = nullptr;

#ifdef ACADEMIA_EXAMPLE
    ExampleGameObject _ExampleObject;
    Player _Player;
    CollisionManager _CollisionManager;

    // Support multiple spawners
    std::vector<std::unique_ptr<Spawner>> _Spawners;
    std::vector<std::unique_ptr<PeriodicTimer>> _SpawnerTimers;
    // one atomic flag per spawner to request spawn from the main thread
    // std::atomic<bool> is not copyable on MSVC; store them via unique_ptr to avoid vector copy issues
    std::vector<std::unique_ptr<std::atomic<bool>>> _SpawnRequested;
#endif

};

