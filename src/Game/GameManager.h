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
#include "external/olc/olcPixelGameEngine.h"

class AcademiaEngine;

class GameManager
{

public:

    void Initialize(AcademiaEngine* engineContext);
    void Update(float elapsedTime);
    void Uninitialize();
    void DrawUI();
    ~GameManager();

    //Color used for UI
    olc::Pixel bgColorNavyBlue = olc::Pixel(18, 52, 74);
    olc::Pixel mainUIOrange = olc::Pixel(255, 140, 66);
    olc::Pixel secondaryUICyan = olc::Pixel(46, 230, 214);
    olc::Pixel alertUIYellow = olc::Pixel(255, 217, 61);

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

