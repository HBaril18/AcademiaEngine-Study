#include "Ennemies.h"
#include "CollisionManager.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include <memory>
#include <cmath>
#include <iostream>

Ennemies::Ennemies(olc::vf2d pos, float radius)
{
    // Initialize object state
    Position = pos;
    Radius = radius;

    collider = std::make_unique<Collider>();

    collider->owner = this;
    collider->position = Position;
    collider->size = Radius;
    collider->layer = 2;
    collider->type = Collider::EColliderType::Circle;
    collider->enabled = true;

    if (collisionManager) {
        collisionManager->RegisterCollider(collider.get());
    }
}

void Ennemies::SetPlayer(const Player* p)
{
    player = p;
    std::cout << "Ennemies::SetPlayer called. Player ptr=" << reinterpret_cast<const void*>(p) << "\n";
}

Ennemies::Ennemies()
{
    // unique_ptr default null
    collisionManager = nullptr;
    // Default properties
    Position = {0.0f, 0.0f};
    Radius = 15.0f;
    Color = olc::DARK_BLUE;
    Health = 50.0f;
}

void Ennemies::InitializeCollision(CollisionManager* collisionManager)
{
    this->collisionManager = collisionManager;
    if (!collider) {
        collider = std::make_unique<Collider>();
    }
    // ensure collider fields are up-to-date
    collider->owner = this;
    collider->position = Position;
    collider->size = Radius;
    collider->layer = 2; // Enemy layer
    collider->type = Collider::EColliderType::Circle;
    collider->enabled = true;
    if (collisionManager) collisionManager->RegisterCollider(collider.get());
}

void Ennemies::ShutdownCollision(CollisionManager* collisionManager)
{
    if (collider) {
        if (collisionManager) collisionManager->UnregisterCollider(collider.get());
        collider.reset();
    }
    this->collisionManager = nullptr;
}

Ennemies::~Ennemies()
{
    if (collisionManager && collider) {
        collisionManager->UnregisterCollider(collider.get());
    }
    // unique_ptr libérera automatiquement
}

void Ennemies::Draw(AcademiaEngine& engine) {
    olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
    // Debug log to ensure draw is called and positions are sane
    engine.FillCircle(pixelPos, static_cast<int32_t>(Radius), Color);
}

const olc::vf2d& Ennemies::GetPlayerPosition(AcademiaEngine& engine) const {
    const Player* playerPtr = GetPlayer();
    if (playerPtr)
        return playerPtr->GetPosition();

    // Fallback: return this enemy's position if no player is set
    return Position;
}

void Ennemies::GetDirection(AcademiaEngine& engine, olc::vf2d playerPosition) {
    // Calculer le vecteur direction = playerPos - Position.
    // Normaliser en évitant la division par zéro qui pourrait produire des NaN.
    olc::vf2d diff = playerPosition - this->Position;
    float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    const float EPS = 1e-6f;
    if (len > EPS) {
        direction = olc::vf2d{ diff.x / len, diff.y / len };
    }
    else {
        // si la distance est nulle, rester immobile (direction nulle)
        direction = olc::vf2d{ 0.0f, 0.0f };
    }
}

void Ennemies::AddForce(AcademiaEngine& engine, float force, olc::vf2d direction, float elapsedTime)
{
    Position += direction * force * elapsedTime;
}

void Ennemies::TakeDamage(float damage) {
    Health -= damage;
    if (Health < 0) {
        Health = 0;
    }
}

void Ennemies::RemoveEnnemie(std::deque<std::unique_ptr<Ennemies>>& ennemies) {
    auto it = std::remove_if(ennemies.begin(), ennemies.end(),
        [](const std::unique_ptr<Ennemies>& e) { return e->Health <= 0; });
    ennemies.erase(it, ennemies.end());
}

void Ennemies::Update(AcademiaEngine& engine, float elapsedTime)
{
    GetDirection(engine, GetPlayerPosition(engine));
    AddForce(engine, 50.0f, direction, elapsedTime);
    if (collider) collider->position = Position;
}
