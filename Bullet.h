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
    void SetSprite(olc::Sprite* sprite)
    {
        spriteSheet = sprite;
    }
    olc::Sprite* sprite = nullptr;
    olc::Decal* decal = nullptr;

protected:
    float AnimTimer = 0.0f;
    int Frame = 0;
    int MaxFrames = 6;
    float AnimSpeed = 12.0f; // frames par seconde

private:
    olc::vf2d direction;
    // store previous position to allow sweep tests (prevent tunneling)
    olc::vf2d previousPosition;
    olc::Pixel Color = olc::Pixel(143, 200, 250);
    float speed = 1200.0f;
	float radius = 5.0f;
    olc::Sprite* spriteSheet = nullptr;
};