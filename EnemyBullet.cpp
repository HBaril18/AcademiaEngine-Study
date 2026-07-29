#include "EnemyBullet.h"

void EnemyBullet::SetPosition(const olc::vf2d& pos)
    {
        Position = pos;
        previousPosition = pos;
    }

olc::vf2d EnemyBullet::GetPosition() const
    {
        return Position;
    }

olc::vf2d EnemyBullet::GetPreviousPosition() const
    {
        return previousPosition;
    }

float EnemyBullet::GetRadius() const
    {
        return radius;
    }

void EnemyBullet::SetDirection(const olc::vf2d& dir)
    {
        direction = dir;
    }

void EnemyBullet::Update(AcademiaEngine& engine, float elapsedTime)
    {
        previousPosition = Position;
        Position += direction * speed * elapsedTime;

        if (collider)
            collider->position = Position;
    }

void EnemyBullet::Draw(AcademiaEngine& engine)
    {
    olc::vf2d imageDimension = { (float)sprite->width, (float)sprite->height };
    olc::vf2d currentDirection = direction.norm();

    const olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);

    constexpr float PI = 3.14159265f;
    // Adjust if your sprite faces up instead of right:

    float angle = atan2f(-currentDirection.y, currentDirection.x) - PI;

    engine.DrawRotatedDecal(
        { (float)pixelPos.x, (float)pixelPos.y },
        decal,
        angle,
        { imageDimension.x / 2, imageDimension.y / 2 }, // center of sprite
        { scale, scale }
    );
    }

void EnemyBullet::InitializeCollision(CollisionManager* collisionManager)
    {
        if (!collider)
        {
            collider = new Collider();
            collider->owner = this;
            collider->position = Position;
            collider->size = radius;
            collider->layer = 4; // EnemyBullet
            collider->type = Collider::EColliderType::Circle;
            collider->enabled = true;

            if (collisionManager)
                collisionManager->RegisterCollider(collider);
        }
    }

void EnemyBullet::ShutdownCollision(CollisionManager* collisionManager)
    {
        if (collider)
        {
            if (collisionManager)
                collisionManager->UnregisterCollider(collider);

            delete collider;
            collider = nullptr;
        }
    }