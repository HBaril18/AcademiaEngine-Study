#include "Bullet.h"
#include "CollisionManager.h"
#include "Collider.h"

void Bullet::Draw(AcademiaEngine& engine) {
	if (!spriteSheet) return;

    olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
    //engine.FillCircle(pixelPos, static_cast<int32_t>(radius), Color);
	int frameWidth = 160;  // adapte à ton sprite
	int frameHeight = 141;

	int x = Frame * frameWidth;
	int y = 0;

	engine.DrawPartialSprite(
		pixelPos.x - frameWidth / 2,
		pixelPos.y - frameHeight / 2,
		spriteSheet,
		x, y,
		frameWidth,
		frameHeight
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
