#include "Bullet.h"
#include "CollisionManager.h"
#include "Collider.h"

void Bullet::Draw(AcademiaEngine& engine) {
    olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
    engine.FillCircle(pixelPos, static_cast<int32_t>(radius), Color);
}

void Bullet::Update(AcademiaEngine& engine, float elapsedTime)
{
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
