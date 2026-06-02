#pragma once
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include "Ennemies.h"

class AcademiaEngine;

class Spawner
{
public:
    virtual ~Spawner() = default;

    virtual void Update(AcademiaEngine& engine, float elapsedTime) {}
    //virtual void Draw(AcademiaEngine& engine) {}

    const olc::vf2d& GetPosition() const { return Position; }
    void SetPosition(const olc::vf2d& position) { Position = position; }

    void SpawnEnnemies(AcademiaEngine& engine);
    std::deque<Ennemies>& GetEnnemies() { return ennemies; }

protected:
    olc::vf2d Position;
    std::deque<Ennemies> ennemies;
};
