#pragma once
#include <deque>
#include <memory>
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include "Ennemies.h"
#include "Collider.h"
#include "CollisionManager.h"

class AcademiaEngine;

class Spawner
{
public:
    void SetGameManager(GameManager* gm) { gameManager = gm; }

    void Update(AcademiaEngine& engine, float elapsedTime);

    const olc::vf2d& GetPosition() const { return Position; }

    void SetPosition(const olc::vf2d& position) { Position = position; }

private:
    olc::vf2d Position;

    float SpawnTimer = 0.0f;
    float SpawnInterval = 5.0f;
    GameManager* gameManager = nullptr;
};
