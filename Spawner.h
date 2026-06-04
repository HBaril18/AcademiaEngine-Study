#pragma once
#include <deque>
#include <memory>
#include <mutex>
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include "Ennemies.h"
#include "Collider.h"
#include "CollisionManager.h"

class AcademiaEngine;

class Spawner
{
public:
    virtual ~Spawner() = default;

    virtual void Update(AcademiaEngine& engine, float elapsedTime) {}
    //virtual void Draw(AcademiaEngine& engine) {}

    const olc::vf2d& GetPosition() const { return Position; }
    void SetPosition(const olc::vf2d& position) { Position = position; }

    void SpawnEnnemies(AcademiaEngine& engine, Player* player, CollisionManager* collisionManager);
    // Accessor returning a reference to the internal ennemies container.
    std::deque<std::unique_ptr<Ennemies>>& GetEnnemies() { return ennemies_container; }
    // Provide access to the internal mutex protecting the ennemies container
    std::mutex& GetEnnemiesMutex() { return ennemies_mutex; }
    // Allow external code to set the CollisionManager used by this spawner
    void SetCollisionManager(CollisionManager* cm) { collisionManager = cm; }

protected:
    olc::vf2d Position;
    std::deque<std::unique_ptr<Ennemies>> ennemies_container;

private:
    // Protect concurrent access to ennemies_container
    std::mutex ennemies_mutex;
    CollisionManager* collisionManager = nullptr;
};
