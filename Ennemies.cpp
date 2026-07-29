#include "Ennemies.h"
#include "CollisionManager.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include <memory>
#include <cmath>
#include <iostream>

Ennemies::Ennemies(olc::vf2d pos, float radius, float maxH)
{
    // Initialize object state
    Position = pos;
    Radius = radius;
    MaxHealth = maxH;
    Health = MaxHealth;

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
Ennemies::Ennemies()
{
    // unique_ptr default null
    collisionManager = nullptr;
    // Default properties
    Position = {0.0f, 0.0f};
    Radius = 15.0f;
    Color = olc::DARK_BLUE;
    Health = MaxHealth;
}

void Ennemies::SetPlayer(Player* p)
{
    player = p;
}

void Ennemies::SetMaxHealth(float h) {
    MaxHealth = h;
    Health = MaxHealth;
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

void Ennemies::Draw(AcademiaEngine& engine)
{
    olc::Sprite* sheet = GetSprite(engine);
    if (!sheet) return;

    int frameWidth = 48;
    int frameHeight = 48;

    int x = Frame * frameWidth;
    int y = 0;

    const olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);

    int scale = GetScale();

    engine.DrawPartialSprite(
        pixelPos.x - (frameWidth * scale) / 2,
        pixelPos.y - (frameHeight * scale) / 2,
        sheet,
        x, y,
        frameWidth, frameHeight, scale
    );

    engine.FillRect(pixelPos + olc::vi2d(-20, -30), { 40, 5 }, olc::WHITE);

    if (GetHealth() > 0) {
        int healthWidth = (int)(40 * (Health / MaxHealth));
        engine.FillRect(pixelPos + olc::vi2d(-20, -30), { healthWidth, 5 }, olc::RED);
    }
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

void Ennemies::TakeDamage(float damage, float elapsedTime) {
    float before = Health;
    Health -= damage;
    if (Health <= 0.0f) {
        Health = 0.0f;
        Player* p = GetPlayer();
        p->AddScore(100.0f);

        // disable collider immediately to avoid further collision processing
        if (collider) collider->enabled = false;
    }
}

void Ennemies::RemoveEnnemie(
    std::deque<std::unique_ptr<Ennemies>>& ennemies,
    AcademiaEngine& engine)
{
    ennemies.erase(
        std::remove_if(ennemies.begin(), ennemies.end(),
            [&](const std::unique_ptr<Ennemies>& e)
            {
                if (e->Health <= 0)
                {
					//Random number to get a chance to spawn a powerup when an enemy dies
					float temp = (float)(rand() % 100) / 100.0f;
					if (temp < 0.25f) e->SpawnPowerUp(engine);
                    return true;
                }
                return false;
            }),
        ennemies.end()
    );
}

void Ennemies::SpawnPowerUp(AcademiaEngine& engine) {
    //Random number to spawn different types of powerups
    //Each powerup has different chances to spawn
    float temp = (float)(rand() % 100) / 100.0f;
    auto p = std::make_unique<PowerUp>(Position, Radius, PowerUpType::Shield);
    if (temp < 0.5f) {
        p = std::make_unique<PowerUp>(Position, Radius, PowerUpType::Heal);
    }
    else if (temp < 0.7f) {
        p = std::make_unique<PowerUp>(Position, Radius, PowerUpType::Speed);
    }
    else if (temp < 0.9f) {
        p = std::make_unique<PowerUp>(Position, Radius, PowerUpType::Damage);
    }

    p->sprite = &engine.PowerUpSheet;
    p->decal = new olc::Decal(p->sprite);

    p->SetPosition(Position);

    if (collisionManager)
    {
        p->InitializeCollision(collisionManager);
    }

    if (p->collider)
    {
        p->collider->position = p->GetPosition();
    }
    if (gameManager) 
    {
        p->SetGameManager(gameManager);
    }
    gameManager->GetPowerUpList().push_back(std::move(p));
}

void Ennemies::Update(AcademiaEngine& engine, float dt)
{
    if (player)
    {
        GetDirection(engine, player->GetPosition());
    }

    Timer += dt;

    if (Timer > 0.2f)
    {
        Frame = (Frame + 1) % 4; // 4 frames animation
        Timer = 0.f;
    }

    // Mouvement de base
    AddForce(engine, GetSpeed(), direction, dt);

    // Knockback
    Position += recoilVelocity * dt;
    recoilVelocity *= 0.85f;

    if (recoilVelocity.mag2() < 1.0f)
        recoilVelocity = { 0.0f, 0.0f };

    //Collider
    if (collider)
        collider->position = Position;
}
