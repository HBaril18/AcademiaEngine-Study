#pragma once
#include "Ennemies.h"
#include "CollisionManager.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

enum class CthulhuMovementState
{
    Intro,
    Orbit,
    Sweep,
    Teleport,
    Summon
};

class CollisionManager;
struct Collider;

class Cthulhu : public Ennemies
{
public:
    ~Cthulhu();
    Cthulhu(olc::vf2d pos);

    void SetGameManager(GameManager* gm) { gameManager = gm; }

    virtual void InitializeCollision(CollisionManager* collisionManager) override;
    virtual void Update(AcademiaEngine& engine, float elapsedTime) override;
    virtual void Draw(AcademiaEngine& engine) override;
	virtual void TakeDamage(float damage, float elapsedTime) override;


    void DrawBossHealthBar(AcademiaEngine& engine, float health, float maxHealth);

    void UpdateIntro(float elapsedTime, AcademiaEngine& engine);
	void UpdateOrbit(float elapsedTime);
	void UpdateSweep(float elapsedTime, AcademiaEngine& engine);
	void UpdateTeleport(float elapsedTime);
	void UpdateSummons(float elapsedTime, AcademiaEngine& engine);
	void UpdateFigure8(float elapsedTime);

    bool TryGetRandomSpawnPositionAround(
        AcademiaEngine& engine,
        float minDistance,
        float maxDistance,
        float spawnRadius,
        const std::vector<olc::vf2d>& plannedPositions,
        olc::vf2d& outPosition
    );
    void SummonEnemiesAround(AcademiaEngine& engine);
    olc::vf2d GetRandomPointAround(float minDistance, float maxDistance);
    bool IsSpawnPositionValid(AcademiaEngine& engine, const olc::vf2d& spawnPos, float spawnRadius, const std::vector<olc::vf2d>& plannedPositions);

    bool IsDialogueActive() const override;
	void DrawDialogue(AcademiaEngine& engine, std::vector<std::string> dialogue) override;
    void UpdateDialogue(float elapsedTime, std::vector<std::string> dialogue);

    float RandomFloat(float min, float max)
    {
        return min + (float(rand()) / float(RAND_MAX)) * (max - min);
    }
    bool CirclesOverlap(const olc::vf2d& aPos, float aRadius, const olc::vf2d& bPos, float bRadius)
    {
        float radiusSum = aRadius + bRadius;
        return (aPos - bPos).mag2() < radiusSum * radiusSum;
    }

    void SetColliderEnabled(bool enabled)
    {
        if (collider)
        {
            collider->enabled = enabled;
        }
    }

    virtual float GetSpeed() const override { return 40.0f; }
    virtual float GetDamage() const override { return 25.0f; }

    olc::Sprite* GetSprite(AcademiaEngine& engine)
    {
        return &engine.EyeChtulhuSheet;
    }

    float DialogueTimer = 0.0f;

    olc::vf2d topCenterWorld;

private:
    CthulhuMovementState _MovementState = CthulhuMovementState::Intro;

    olc::vf2d _TargetIntroPosition;
    int _DialogueIndex;
    bool _IntroInitialized = false;
    bool _ShowDialogue;
    float _CharacterTimer = 0.0f;
    int _VisibleCharacters = 0;

    float _MovementStateTimer = 0.0f;
    float _OrbitAngle = 0.0f;
    float _TeleportTimer = 0.0f;
    float _SummonTimer = 0.0f;
    float _SweepDirection = 1.0f;
    float _PatternTimer = 0.0f;
    float _DeathTimer = 0.0f;

    float _LookAngle = 0.0f;
    olc::vf2d _PreviousPosition;

    int _CurrentFrame = 0;
    float _AnimationTimer = 0.0f;
    float _AnimationSpeed = 0.1f;

    bool _CanTakeDamage = false;

    // Collision
    Collider* collider = nullptr;
    CollisionManager* collisionManager = nullptr;
};