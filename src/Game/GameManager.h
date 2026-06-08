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
    void GameLogic(float elapsedTime);
    void RestartGame();
	void AddScore(float scoreToAdd) { _Score += scoreToAdd; }
	float GetScore() const { return _Score; }
	void SetScore(float score) { _Score = score; }
    ~GameManager();

    //Color used for UI
    const olc::Pixel bgColorNavyBlue = olc::Pixel(18, 52, 74);
    const olc::Pixel mainUIOrange = olc::Pixel(255, 140, 66);
    const olc::Pixel secondaryUILightBlue = olc::Pixel(155, 92, 255);
    const olc::Pixel alertUIYellow = olc::Pixel(255, 217, 61);

private:

    AcademiaEngine* _EngineContext = nullptr;

	float _Score = 0.0f;
    // Non-blocking game over state flag
    bool _IsGameOver = false;
    // Whether spawners are currently paused due to game over
    bool _SpawnersPaused = false;
    // Game over transition (fade) parameters
    float _GameOverFade = 0.0f; // 0.0 = no overlay, 1.0 = full overlay
    float _GameOverFadeDuration = 1.0f; // seconds for fade in/out
    bool _IsFadingIn = false;
    bool _IsFadingOut = false;

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

