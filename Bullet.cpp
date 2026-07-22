#include "Bullet.h"
#include "CollisionManager.h"
#include "Collider.h"

void Bullet::Draw(AcademiaEngine& engine) {
	olc::vf2d imageDimension = { (float)sprite->width, (float)sprite->height };
	olc::vf2d direction = GetDirection(); //Cursor direction
	direction = direction.norm();

	const olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);

	constexpr float PI = 3.14159265f;
	// Adjust if your sprite faces up instead of right:

	float angle = atan2f(-direction.y, direction.x) + PI / 2;

	engine.DrawRotatedDecal(
		{ (float)pixelPos.x, (float)pixelPos.y },
		decal,
		angle,
		{ imageDimension.x / 2, imageDimension.y / 2 } // center of sprite
	);
}

void Bullet::Update(AcademiaEngine& engine, float elapsedTime)
{
	AnimTimer += elapsedTime;

	Frame = (int)(AnimTimer * AnimSpeed) % MaxFrames;

	// Save previous position for sweep collision tests
	previousPosition = Position;
	Position += direction * speed * elapsedTime;
	if (collider) collider->position = Position;
}

void Bullet::SetDirection(const olc::vf2d& dir)
{
    direction = dir;
}

olc::vf2d Bullet::GetDirection()
{
	return direction;
}

void Bullet::InitializeCollision(CollisionManager* collisionManager) {
	if (!collider) {
		collider = new Collider();
		collider->owner = this;
		collider->position = Position;
		collider->size = radius;
		collider->layer = 3; // Bullet layer
		collider->type = Collider::EColliderType::Circle;
		collider->enabled = true;
		if (collisionManager) collisionManager->RegisterCollider(collider);
	}
}

void Bullet::ShutdownCollision(CollisionManager* collisionManager) {
	if (collider) {
		if (collisionManager) collisionManager->UnregisterCollider(collider);
		delete collider;
		collider = nullptr;
	}
}
