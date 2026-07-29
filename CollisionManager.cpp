#include "CollisionManager.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Player.h"
#include "Ennemies.h"
#include "Bullet.h"
#include "PowerUp.h"
#include <memory>

CellKey CollisionManager::GetCell(
	const olc::vf2d& pos)
{
	return
	{
		static_cast<int>(
			floor(pos.x / CELL_SIZE)
		),

		static_cast<int>(
			floor(pos.y / CELL_SIZE)
		)
	};
}

void CollisionManager::BuildGrid()
{
	grid.clear();

	for (Collider* collider : colliders)
	{
		if (!collider)
			continue;

		if (!collider->enabled)
			continue;

		float r = collider->size;

		float minX = collider->position.x - r;
		float maxX = collider->position.x + r;

		float minY = collider->position.y - r;
		float maxY = collider->position.y + r;

		int startCellX =
			static_cast<int>(std::floor(minX / CELL_SIZE));

		int endCellX =
			static_cast<int>(std::floor(maxX / CELL_SIZE));

		int startCellY =
			static_cast<int>(std::floor(minY / CELL_SIZE));

		int endCellY =
			static_cast<int>(std::floor(maxY / CELL_SIZE));

		for (int y = startCellY; y <= endCellY; y++)
		{
			for (int x = startCellX; x <= endCellX; x++)
			{
				grid[{x, y}].push_back(collider);
			}
		}
	}
}

std::vector<Collider*> CollisionManager::GetNearbyColliders(
	const olc::vf2d& position)
{
	std::vector<Collider*> result;
	std::unordered_set<Collider*> seen;

	CellKey center = GetCell(position);

	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			CellKey key =
			{
				center.x + x,
				center.y + y
			};

			auto it = grid.find(key);

			if (it == grid.end())
				continue;

			for (Collider* c : it->second)
			{
				if (seen.insert(c).second)
				{
					result.push_back(c);
				}
			}
		}
	}

	return result;
}

void CollisionManager::RegisterCollider(Collider* collider)
{
	colliders.push_back(collider);
}

void CollisionManager::RemoveBullet(Bullet* b)
{
	if (!b)
		return;

	if (b->markedForRemoval)
		return;

	b->markedForRemoval = true;

	if (b->collider)
		b->collider->enabled = false;
}

void CollisionManager::RemoveColliderPowerUp(PowerUp* p)
{
	if (!p) return;

	if (p->collider)
	{
		if (!p->collider->enabled)
			return;

		p->collider->enabled = false;
	}

	pendingPowerUpRemovals.push_back(p);
}

Player* CollisionManager::GetPlayer()
{
	if (player) return player;

	for (auto *c : colliders) {
		if (c && c->owner && c->layer == 1) {
			return static_cast<Player*>(c->owner);
		}
	}
	return nullptr;
}

std::vector<Bullet*> CollisionManager::GetBullets()
{
	std::vector<Bullet*> result;
	if (bullets) {
		for (auto &b : *bullets) {
			result.push_back(&b);
		}
		return result;
	}

	for (auto *c : colliders) {
		if (c && c->owner && c->layer == 3) {
			result.push_back(static_cast<Bullet*>(c->owner));
		}
	}
	return result;
}

std::vector<Ennemies*> CollisionManager::GetEnnemies()
{
	std::vector<Ennemies*> result;
	if (ennemies) {
		for (auto &e : *ennemies) {
			result.push_back(e.get());
		}
		return result;
	}

	// fallback: infer enemies from colliders registered
	for (auto *c : colliders) {
		if (c && c->owner && c->layer == 2) {
			result.push_back(static_cast<Ennemies*>(c->owner));
		}
	}
	return result;
}

std::vector<PowerUp*> CollisionManager::GetPowerUp()
{
	std::vector<PowerUp*> result;
	if (powerUp) {
		for (auto& e : *powerUp) {
			result.push_back(e.get());
		}
		return result;
	}

	// fallback: infer enemies from colliders registered
	for (auto* c : colliders) {
		if (c && c->owner && c->layer == 2) {
			result.push_back(static_cast<PowerUp*>(c->owner));
		}
	}
	return result;
}

void CollisionManager::UnregisterCollider(Collider* collider)
{
	// Implementation for unregistering a collider
	if (!collider) {
		return; // Handle null pointer case
	}
	auto it = std::remove(colliders.begin(), colliders.end(), collider);
	if (it != colliders.end()) {
		colliders.erase(it, colliders.end());
	}
}

void CollisionManager::Update(float elapsedTime)
{
	std::unordered_set<uint64_t> processedPairs;

	int bulletCount = 0;
	int enemyCount = 0;

	for (auto* c : colliders)
	{
		if (c->layer == 2) enemyCount++;
		if (c->layer == 3) bulletCount++;
	}

	BuildGrid();
	int collCount = static_cast<int>(colliders.size());
	for (Collider* colliderA : colliders)
	{
		if (!colliderA->enabled)
			continue;

		auto nearby =
			GetNearbyColliders(
				colliderA->position);

		for (Collider* colliderB :
			nearby)
		{
			//CHECKS IF ITS NOT THE SAME COLLIDER
			if (colliderA == colliderB)
				continue;

			uintptr_t low =
				reinterpret_cast<uintptr_t>(
					std::min(colliderA, colliderB));

			uintptr_t high =
				reinterpret_cast<uintptr_t>(
					std::max(colliderA, colliderB));

			uint64_t key =
				((uint64_t)low) ^
				(((uint64_t)high) << 1);

			if (!processedPairs.insert(key).second)
			{
				continue;
			}

			//BASICS CHECKS
			if (!colliderA->enabled || !colliderB->enabled) continue;
			if (colliderA->owner == nullptr || colliderB->owner == nullptr) continue;
			if (!player) continue;
			if (colliderA->layer < 0 || colliderA->layer >= 5 || colliderB->layer < 0 || colliderB->layer >= 5){ continue; }

			int a = colliderA->layer;
			int b = colliderB->layer;
			if (!collisionMatrix[a][b]) continue;

			//SWEEP TEST FOR FAST BULLETS
			// Circle vs Circle collision logic
			if (colliderA->type == Collider::EColliderType::Circle && colliderB->type == Collider::EColliderType::Circle) {
				olc::vf2d delta = colliderB->position - colliderA->position;
				float distanceSquared = delta.mag2();
				float radiusSum = colliderA->size + colliderB->size;
				// Standard overlap test
				bool overlap = (distanceSquared < radiusSum * radiusSum);
				// Additional sweep/tunneling test for bullet vs enemy: perform segment-circle test
				if (!overlap) {
					// Check for enemy-bullet pair and attempt sweep test
					bool isEnemyBullet = ((a == 2 && b == 3) || (a == 3 && b == 2));
					if (isEnemyBullet) {
						Ennemies* enemy = nullptr;
						Bullet* bullet = nullptr;
						if (a == 2) { enemy = static_cast<Ennemies*>(colliderA->owner); bullet = static_cast<Bullet*>(colliderB->owner); }
						else { enemy = static_cast<Ennemies*>(colliderB->owner); bullet = static_cast<Bullet*>(colliderA->owner); }
						if (enemy && bullet) {
							if (!bullet ||
								bullet->markedForRemoval ||
								!bullet->collider ||
								!bullet->collider->enabled)
							{
								continue;
							}
							// segment from previousPosition to current Position
							olc::vf2d p1 = bullet->GetPreviousPosition();
							olc::vf2d p2 = bullet->GetPosition();
							olc::vf2d c = enemy->GetPosition();
							olc::vf2d d = p2 - p1;
							float len2 = d.mag2();
							if (len2 > 0.0f) {
								float t = ((c - p1).dot(d)) / len2;
								t = std::fmax(0.0f, std::fmin(1.0f, t));
								olc::vf2d closest = p1 + d * t;
								float dist2 = (closest - c).mag2();
								float radius =
									enemy->GetRadius() +
									bullet->GetRadius();

								if (dist2 < radius * radius)
								{
									overlap = true;
								}
							}
						}
					}
				}
				if (overlap) {
					// Player-Enemy
					if ((a == 1 && b == 2) || (a == 2 && b == 1)) {
						Player* p = player;
						Ennemies* e = nullptr;

						if (a == 2)
							e = static_cast<Ennemies*>(colliderA->owner);
						else
							e = static_cast<Ennemies*>(colliderB->owner);

						if (p && e && p->damageCooldown <= 0.0f)
						{
							//Damage
							if (p->GetShield() > 0) {
								p->RemoveShield();
							}
							else {
								p->TakeDamage(10.0f);
							}
							p->damageCooldown = p->damageDelay;

							//Knockback direction
							olc::vf2d dir = p->GetPosition() - e->GetPosition();

							if (dir.mag2() > 0.001f)
								dir = dir.norm();

							//Apply force
							float knockbackStrength = 550.0f;
							p->knockbackVelocity += dir * knockbackStrength;
							//Ennemie Knockback
							e->recoilVelocity -= dir * 300.0f;
						}
					}
					// Enemy-Bullet
					else if ((a == 2 && b == 3) || (a == 3 && b == 2))
					{
						Ennemies* enemy = nullptr;
						Bullet* bullet = nullptr;

						if (a == 2)
						{
							enemy = static_cast<Ennemies*>(colliderA->owner);
							bullet = static_cast<Bullet*>(colliderB->owner);
						}
						else
						{
							enemy = static_cast<Ennemies*>(colliderB->owner);
							bullet = static_cast<Bullet*>(colliderA->owner);
						}

						if (!enemy || !bullet)
							continue;

						if (bullet->markedForRemoval ||
							!bullet->collider ||
							!bullet->collider->enabled)
						{
							continue;
						}

						enemy->TakeDamage(
							10.0f * player->GetDamageMultiplier(),
							elapsedTime
						);

						RemoveBullet(bullet);
						break;
					}
					// EnemyBullet_Player
					else if ((a == 4 && b == 1) || (a == 1 && b == 4))
					{
						EnemyBullet* enemyBullet = nullptr;
						Player* player = nullptr;

						if (a == 4)
						{
							enemyBullet = static_cast<EnemyBullet*>(colliderA->owner);
							player = static_cast<Player*>(colliderB->owner);
						}
						else
						{
							enemyBullet = static_cast<EnemyBullet*>(colliderB->owner);
							player = static_cast<Player*>(colliderA->owner);
						}

						if (!enemyBullet || !player)
							continue;

						if (enemyBullet->markedForRemoval ||
							!enemyBullet->collider ||
							!enemyBullet->collider->enabled)
						{
							continue;
						}
						if (player->damageCooldown <= 0.0f)
						{
								if (player->GetShield() > 0)
								{
									player->RemoveShield();
								}
								else
								{
									player->TakeDamage(enemyBullet->damage);
								}

								player->damageCooldown = player->damageDelay;
						}

						enemyBullet->markedForRemoval = true;

							if (enemyBullet->collider)
								enemyBullet->collider->enabled = false;
					}
					// Player-PowerUp
					else if ((a == 1 && b == 0) || (a == 0 && b == 1)) {
						Player* player = nullptr;
						PowerUp* powerUp = nullptr;
						if (a == 1) {
							player = static_cast<Player*>(colliderA->owner);
							powerUp = static_cast<PowerUp*>(colliderB->owner);
						}
						else {
							powerUp = static_cast<PowerUp*>(colliderA->owner);
							player = static_cast<Player*>(colliderB->owner);
						}
						if (powerUp && player) {
							powerUp->Apply(*player);
							powerUp->collected = true;
							powerUp->markedForRemoval = true;
							RemoveColliderPowerUp(powerUp);
						}
					}
				}
			}
			/*CONTINUE HERE*/
		}
	}

	for (auto* p : pendingPowerUpRemovals)
	{
		if (!p)
			continue;

		p->ShutdownCollision(this);
	}

	pendingPowerUpRemovals.clear();
}