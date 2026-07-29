#pragma once
#include "Collider.h"
#include "Character.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include "CollisionManager.h"

// Forward declaration
class GameManager;
class CollisionManager;
struct Collider;

class EnemyBullet : public Character
{
public:
    //VARIABLES
    olc::vf2d Position;
    olc::vf2d previousPosition;
    olc::vf2d direction;

    float speed = 650.0f;
    float radius = 9.0f;
    float damage = 15.0f;
    float scale = 1.7f;

    bool markedForRemoval = false;

    Collider* collider = nullptr;

    olc::Sprite* sprite = nullptr;
    olc::Decal* decal = nullptr;

    //FUNCTIONS
    void SetPosition(const olc::vf2d& pos);
    olc::vf2d GetPosition() const;
    olc::vf2d GetPreviousPosition() const;

    float GetRadius() const;

    void SetDirection(const olc::vf2d& dir);

    void Update(AcademiaEngine& engine, float elapsedTime);

    void Draw(AcademiaEngine& engine);

    void InitializeCollision(CollisionManager* collisionManager);

    void ShutdownCollision(CollisionManager* collisionManager);

    void SetSprite(olc::Sprite* sprite)
    {
        _SpriteSheet = sprite;
    }

    void SetSpeed(float s) {
        speed = s;
    }
    void SetDamage(float d) {
        damage = d;
    }
private:
    olc::Sprite* _SpriteSheet = nullptr;
};