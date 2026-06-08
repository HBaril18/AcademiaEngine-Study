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
    // previous position accessor for sweep tests
    void SetPreviousPosition(const olc::vf2d& p) { previousPosition = p; }
    olc::vf2d GetPreviousPosition() const { return previousPosition; }
    float GetRadius() const { return radius; }

private:
    olc::vf2d direction;
    // store previous position to allow sweep tests (prevent tunneling)
    olc::vf2d previousPosition;
    olc::Pixel Color = olc::Pixel(255, 157, 59);
    float speed = 1200.0f;
	float radius = 5.0f;
};