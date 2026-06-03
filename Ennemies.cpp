#include "Ennemies.h"
#include "CollisionManager.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"

Ennemies::Ennemies(olc::vf2d pos, float radius)
{
	collider = new Collider();

	collider->owner = this;
	collider->position = pos;
	collider->size = radius;
	collider->layer = 2;
	collider->type = Collider::EColliderType::Circle;
	collider->enabled = true;

	if (collisionManager) {
		collisionManager->RegisterCollider(collider);
	}
}

Ennemies::Ennemies()
{
	collider = nullptr;
	collisionManager = nullptr;
	// Default properties
	Position = {0.0f, 0.0f};
	Radius = 15.0f;
	Color = olc::DARK_BLUE;
	Health = 50.0f;
}

void Ennemies::InitializeCollision(CollisionManager* collisionManager)
{
	this->collisionManager = collisionManager;
	if (!collider) {
		collider = new Collider();
		collider->owner = this;
		collider->position = Position;
		collider->size = Radius;
		collider->layer = 2; // Enemy layer
		collider->type = Collider::EColliderType::Circle;
		collider->enabled = true;
		if (collisionManager) collisionManager->RegisterCollider(collider);
	}
}

void Ennemies::ShutdownCollision(CollisionManager* collisionManager)
{
	if (collider) {
		if (collisionManager) collisionManager->UnregisterCollider(collider);
		delete collider;
		collider = nullptr;
	}
	this->collisionManager = nullptr;
}

Ennemies::~Ennemies()
{
	if (collisionManager) {
		collisionManager->UnregisterCollider(collider);
	}
	delete collider;
}

void Ennemies::Draw(AcademiaEngine& engine) {
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
	engine.FillCircle(pixelPos, static_cast<int32_t>(Radius), Color);
}

const olc::vf2d& Ennemies::GetPlayerPosition(AcademiaEngine& engine) const {
	const Player* playerPtr = GetPlayer();
	if (playerPtr)
		return playerPtr->GetPosition();

	// Fallback: return this enemy's position if no player is set
	return Position;
}

void Ennemies::GetDirection(AcademiaEngine& engine, olc::vf2d playerPosition) {
	//Calculer le vecteur direction = playerPos - Position.
	direction = (playerPosition - this->Position).norm();
}

void Ennemies::AddForce(AcademiaEngine& engine, float force, olc::vf2d direction, float elapsedTime)
{
	Position += direction * force * elapsedTime;
}

void Ennemies::TakeDamage(float damage) {
	Health -= damage;
	if (Health < 0) {
		Health = 0;
	}
}

void Ennemies::RemoveEnnemie(std::deque<Ennemies>& ennemies) {
	auto it = std::remove_if(ennemies.begin(), ennemies.end(),
		[](const Ennemies& e) { return e.Health <= 0; });
	ennemies.erase(it, ennemies.end());
}

void Ennemies::Update(AcademiaEngine& engine, float elapsedTime)
{
	GetDirection(engine, GetPlayerPosition(engine));
	AddForce(engine, 50.0f, direction, elapsedTime);
}
