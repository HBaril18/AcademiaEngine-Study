#pragma once
#include "Character.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"

// Forward declaration
class CollisionManager;
class Player;
struct Collider;

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

class Bullet : public Character
{
public:
    // Collision
    Collider* collider = nullptr;
    bool markedForRemoval = false;

    void InitializeCollision(class CollisionManager* collisionManager);
    void ShutdownCollision(class CollisionManager* collisionManager);

    olc::vf2d GetDirection();
    void SetDirection(const olc::vf2d& dir);
	void Update(AcademiaEngine& engine, float elapsedTime) override;
    void Draw(AcademiaEngine& engine) override;

private:
    olc::vf2d direction;
    olc::Pixel Color = olc::DARK_YELLOW;
    float speed = 1200.0f;
	float radius = 5.0f;
};