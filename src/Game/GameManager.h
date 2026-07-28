#pragma once
#include <Utils.h>
#include <memory>
#include <atomic>
#include "../../PeriodicTimer.h"
#include <vector>
#include "Explosion.h"

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
#include "../../EngineState.h"

class AcademiaEngine;
class Spawner;

enum class EEnemyType
{
    Basic,
    Fast,
    Tank
};

enum class ESpawnPattern
{
    Circular,
    Linear,
    RadialBurst
};

enum class EDifficultyLevel
{
    Easy,
    Medium,
    Hard
};

struct PowerUpNotification
{
    std::string text;
    float timer = 2.0f;
    float maxTimer = 2.0f;
};

struct FallingPixel
{
    olc::vi2d pos;
    olc::Pixel color;
    float velocity;
};

enum class EGameState { Playing, GameOver };

class GameManager
{

public:
    GameManager(AcademiaEngine* engine);
    bool Initialize(AcademiaEngine* engineContext);
    void SetUpControl(AcademiaEngine* engineContext);
    void Update(float elapsedTime);
    bool Uninitialize();
    void DrawUI();
    void EndGameLogic(float elapsedTime);
    void InitializeGame();
	void SetDifficultyLevel(EDifficultyLevel level) { _DifficultyLevel = level; }
	std::string GetDifficultyLevelString() const
	{
		switch (_DifficultyLevel)
		{
		case EDifficultyLevel::Easy:
			return "Easy";
		case EDifficultyLevel::Medium:
			return "Medium";
		case EDifficultyLevel::Hard:
			return "Hard";
		default:
			return "Unknown";
		}
	}
    const bool ButtonDetection(const olc::vi2d& buttonPos, const olc::vi2d& buttonSize);
    void DrawButton(olc::vi2d buttonPos, olc::vi2d buttonSize, olc::Pixel color);
    void AddScore(float scoreToAdd) { _Score += scoreToAdd; }
	float GetScore() const { return _Score; }
    void SetScore(float score) { _Score = score; }
    bool SetupSpawner();
    std::vector<std::unique_ptr<PowerUp>>& GetPowerUpList() { return _PowerUps; }
    void AddPowerUpNotification(const std::string& text);
    void StartExitAnimation();
    ~GameManager();
    void SpawnEnemy(
        EEnemyType type,
        const olc::vf2d& position);
    void SpawnRandomEnemy();
    void StartChtulhuFight();
    void DrawPowerUpUI();
    void ResumeGame();
    void DrawPlayerHealthBar(AcademiaEngine* engineContext);

    const std::deque<std::unique_ptr<Ennemies>>& GetEnemies() const 
    {
        return _Enemies;
    }
	void ClearEnnemyPtrs();
	void ClearPowerUpPtrs();

    EDifficultyLevel NextDifficulty()
    {
        int next = static_cast<int>(_DifficultyLevel) + 1;

        if (next > static_cast<int>(EDifficultyLevel::Hard))
            next = 0;

        return static_cast<EDifficultyLevel>(next);
    }

    //Color used for UI
    const olc::Pixel bgColorNavyBlue = olc::Pixel(18, 52, 74);
    const olc::Pixel mainUIOrange = olc::Pixel(255, 140, 66);
    const olc::Pixel secondaryUILightBlue = olc::Pixel(155, 92, 255);
    const olc::Pixel alertUIYellow = olc::Pixel(255, 217, 61);

    //Movement Keys
    const olc::HWButton moveRightButton;
    const olc::HWButton moveLeftButton;
    const olc::HWButton moveUpButton;
    const olc::HWButton moveDownButton;
    const olc::HWButton jumpButton;
    const olc::HWButton sneakButton;

    std::vector<PowerUpNotification> _PowerUpNotifications;
    std::vector<FallingPixel> _FallingPixels;
    EGameState _GameState = EGameState::Playing;

    bool inBossFight = false;

protected:


private:

    AcademiaEngine* _EngineContext = nullptr;

	float _Score = 0.0f;
    // Non-blocking game over state flag
    bool _IsGameOver = false;
	bool _IsGameStarted = false;
    // Whether spawners are currently paused due to game over
    bool _SpawnersPaused = false;
    // Game over transition (fade) parameters
    //➥ 0.0 = no overlay, 1.0 = full overlay
    float _GameOverFade = 0.0f; 
    float _GameOverFadeDuration = 3.0f; // seconds for fade in/out
    bool _IsFadingIn = false;
    bool _IsFadingOut = false;
    bool _OptionsSelected = false;
    bool _Success = true;

    float _GameOverDelay = 1.0f; // seconds
    float _GameOverTimer = 0.0f;
    bool _PlayerDying = false;

    EDifficultyLevel _DifficultyLevel = EDifficultyLevel::Medium;
    std::vector<Explosion> _Explosions;

#ifdef ACADEMIA_EXAMPLE

    ExampleGameObject _ExampleObject;
    Player _Player;
    CollisionManager _CollisionManager;

    // Support multiple spawners
    std::vector<std::unique_ptr<Spawner>> _Spawners;
    std::vector<std::unique_ptr<std::atomic<bool>>> _SpawnRequested;
    std::vector<std::unique_ptr<PowerUp>> _PowerUps;
    std::deque<std::unique_ptr<Ennemies>> _Enemies;
    std::unique_ptr<Ennemies> _Cthulhu;
#endif

};