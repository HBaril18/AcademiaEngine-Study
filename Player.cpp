#include "Player.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include "CollisionManager.h"
#include "AcademiaEngine-Study/../src/Game/GameManager.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

Player::Player(AcademiaEngine& engine)
{
    _Engine = &engine;
    sprite = &_Engine->PlayerSheet;
    decal = new olc::Decal(sprite);

}

void Player::Update(AcademiaEngine& engine, float elapsedTime) {
    if (damageCooldown > 0.0f) {
        damageCooldown -= elapsedTime;
    }
    if (collider) {
        collider->position = Position;
    }

    // Apply knockback
    Position += knockbackVelocity * elapsedTime;

    // Damp knockback over time
    knockbackVelocity *= 0.85f;

    // Stop tiny jitter
    if (knockbackVelocity.mag2() < 1.0f)
        knockbackVelocity = { 0.0f, 0.0f };
}

void Player::InitializeCollision(CollisionManager* collisionManager) {
    if (!collider) {
        collider = new Collider();
        collider->owner = this;
        collider->position = Position;
        collider->size = Radius;
        collider->layer = 1; // Player layer
        collider->type = Collider::EColliderType::Circle;
        collider->enabled = true;
        if (collisionManager) {
            collisionManager->RegisterCollider(collider);
            this->collisionManager = collisionManager;
        }
    }
}

void Player::ShutdownCollision(CollisionManager* collisionManager) {
    if (collider) {
        if (collisionManager) collisionManager->UnregisterCollider(collider);
        delete collider;
        collider = nullptr;
    }
}

void Player::Draw(AcademiaEngine& engine)
{
    olc::vf2d imageDimension = { (float)sprite->width, (float)sprite->height };
    olc::vf2d direction = GetPlayerDirection(engine); //Cursor direction
    direction = direction.norm();

    const olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);

    constexpr float PI = 3.14159265f;
    // Adjust if your sprite faces up instead of right:

    float angle = atan2f(-direction.y, direction.x) + PI / 2;

    engine.DrawRotatedDecal(
        { (float)pixelPos.x, (float)pixelPos.y },
        decal,
        angle,
        { imageDimension.x / 2, imageDimension.y / 2 }, // center of sprite
        { Scale, Scale }
    );
}

void Player::SpawnBullet(AcademiaEngine& engine) {
    // Construct the bullet directly in the container to ensure its collider owner is the stored object
    bullets.emplace_back();
    Bullet& bullet = bullets.back();
    bullet.SetSprite(&engine.BulletSheet);
    bullet.SetPosition(Position); // Set the bullet's initial position to the player's position
    //Sprite and Decal
    bullet.sprite = &_Engine->BulletSheet;
    bullet.decal = new olc::Decal(bullet.sprite);
    // initialize previous position to same as spawn to avoid invalid sweep tests on first update
    bullet.SetPreviousPosition(Position);

    // Initialize bullet collision if player has collision manager set
    if (collisionManager) {
        bullet.InitializeCollision(collisionManager);
    }
    // ensure collider position up-to-date immediately after spawn
    if (bullet.collider) bullet.collider->position = bullet.GetPosition();

    olc::vf2d direction = GetPlayerDirection(engine);

    if (direction.mag() > 0.0f)
        direction = direction.norm();
    else
        direction = { 1.0f, 0.0f }; // default direction
    bullet.SetDirection(direction);
    //std::cout << "Bullet deque" << bullets.size() << std::endl;
}

olc::vf2d Player::GetPlayerDirection(AcademiaEngine& engine) const {
    olc::vf2d cursorWorldPos = GetCursorPosition(engine);
    olc::vf2d direction = cursorWorldPos - Position;
    return direction;
}

void Player::DrawCursor(AcademiaEngine& engine, olc::vf2d cursorWorldPos) {
    olc::vi2d cursorPixelPos = engine.ConvertWorldPositionToPixels(cursorWorldPos);
    engine.DrawCircle(cursorPixelPos, 5, olc::RED);
}

void Player::AddForce(AcademiaEngine& engine, float force, const std::vector<float>& direction, float elapsedTime) {
    for (int i = 0; i < direction.size(); i++) {
        Position.x += direction[0] * force * elapsedTime;
        Position.y += direction[1] * force * elapsedTime;
    }
    olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
}

olc::vf2d Player::GetCursorPosition(AcademiaEngine& engine) const {
    // This will trigger a copy of the returned value from `engine.GetMousePos()`.
    // cursorPixelPos should be a constant reference (const&) to prevent the copy of the vi2d
    const olc::vi2d& cursorPixelPos = engine.GetMousePos();
    
    // Here the copy is fine because ConvertPixelsToWorldPosition will return a copy
    olc::vf2d cursorWorldPos = engine.ConvertPixelsToWorldPosition(cursorPixelPos);
    return cursorWorldPos;
}

void Player::UpdateBullets(AcademiaEngine& engineContext) {
    // remove bullets that are out of screen or whose colliders are disabled
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [&](Bullet& bullet) {
                // Remove bullets explicitly flagged for removal (e.g., hit UI)
                if (bullet.markedForRemoval) {
                    if (collisionManager && bullet.collider) bullet.ShutdownCollision(collisionManager);
                    return true;
                }
                // If player has a collision manager, shutdown bullet collision before removal
                if (collisionManager && bullet.collider && !bullet.collider->enabled) {
                    // ensure we only shutdown once
                    Bullet* bptr = &bullet;
                    bullet.ShutdownCollision(collisionManager);
                    std::cout << "Player: Removed bullet from deque " << bptr << std::endl;
                    return true;
                }
                olc::vi2d pixel = engineContext.ConvertWorldPositionToPixels(bullet.GetPosition());

                bool out = pixel.x < 0 || pixel.x > engineContext.ScreenWidth() ||
                    pixel.y < 0 || pixel.y > engineContext.ScreenHeight();
                if (out && collisionManager) bullet.ShutdownCollision(collisionManager);
                return out;
             }),
         bullets.end()
     );
}

void Player::AddScore(float scoreToAdd) {
    if (gameManager) {
        gameManager->AddScore(scoreToAdd);
    }
}

Player::~Player()
{
    ShutdownCollision(collisionManager);
    delete decal;
}