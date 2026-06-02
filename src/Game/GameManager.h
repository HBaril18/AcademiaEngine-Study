#pragma once
#include <Utils.h>
#include <memory>
#include <atomic>
#include "../../PeriodicTimer.h"

#define ACADEMIA_EXAMPLE

#ifdef ACADEMIA_EXAMPLE
#include "ExampleGameObject.h"
#include "Player.h"
#include "Bullet.h"
#include "Ennemies.h"
#include "Spawner.h"
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
	Bullet _Bullet;
    Ennemies _Ennemies;
    Spawner _Spawner;
    std::unique_ptr<PeriodicTimer> _SpawnerTimer;
    std::atomic<bool> _SpawnRequested{false};
#endif

};

