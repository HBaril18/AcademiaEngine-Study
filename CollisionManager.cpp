#include "CollisionManager.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Player.h"
#include "Ennemies.h"
#include "Bullet.h"
#include "PowerUp.h"
#include <memory>

void CollisionManager::RegisterCollider(Collider* collider)
{
	// Implementation for registering a collider
	if (!collider) {
		return; // Handle null pointer case
	}
	colliders.push_back(collider);
}

void CollisionManager::RemoveBullet(Bullet* b)
{
	if (!b) return;
	// If collider exists and is enabled, disable it immediately to prevent multiple hits in same update pass
	if (b->collider) {
		if (!b->collider->enabled) return; // already scheduled/disabled
		b->collider->enabled = false;
	}
	// mark for removal immediately to help other systems detect it's gone
	b->markedForRemoval = true;
	pendingBulletRemovals.push_back(b);
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
	// Implementation for updating collisions
	int collCount = static_cast<int>(colliders.size());
	for (int i = 0; i < collCount; ++i) {
		for (int j = i + 1; j < collCount; ++j) {
			Collider* colliderA = colliders[i];
			Collider* colliderB = colliders[j];
			if (!colliderA || !colliderB) continue;
			// Check if both colliders are enabled
			if (!colliderA->enabled || !colliderB->enabled) continue;
			if (colliderA->owner == nullptr || colliderB->owner == nullptr) continue;
			if (!player) continue; // player must be set to handle player collision

			// layer bounds check
			if (colliderA->layer < 0 || colliderA->layer >= 4 || colliderB->layer < 0 || colliderB->layer >= 4) continue;

			const bool collisionMatrix[4][4] = {
				//0     1      2      3
				{false, false, false, false}, // 0 PowerUp
				{false, false, true,  false}, // 1 Player
				{false, true,  false, true }, // 2 Enemy
				{false, false, true,  false}  // 3 Bullet
			};

			int a = colliderA->layer;
			int b = colliderB->layer;
			if (!collisionMatrix[a][b]) continue;

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
								if (dist2 < enemy->GetRadius() * enemy->GetRadius()) {
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
							p->TakeDamage(10.0f);
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
					else if ((a == 2 && b == 3) || (a == 3 && b == 2)) {
						Ennemies* enemy = nullptr;
						Bullet* bullet = nullptr;
						if (a == 2) {
							enemy = static_cast<Ennemies*>(colliderA->owner);
							bullet = static_cast<Bullet*>(colliderB->owner);
						} else {
							enemy = static_cast<Ennemies*>(colliderB->owner);
							bullet = static_cast<Bullet*>(colliderA->owner);
						}
						if (enemy) {
							enemy->TakeDamage(10.0f, elapsedTime);
						}
						if (bullet) {
							RemoveBullet(bullet);
						}
					}
					// Player-PowerUp
					else if ((a == 1 && b == 0) || (a == 0 && b == 1)) {
						Player* player = nullptr;
						PowerUp* powerUp = nullptr;
						if (a == 2) {
							player = static_cast<Player*>(colliderA->owner);
							powerUp = static_cast<PowerUp*>(colliderB->owner);
						}
						else {
							powerUp = static_cast<PowerUp*>(colliderB->owner);
							player = static_cast<Player*>(colliderA->owner);
						}
					}
				}
			}

			// Box vs Box
			if (colliderA->type == Collider::EColliderType::Box && colliderB->type == Collider::EColliderType::Box) {
				olc::vf2d delta = colliderB->position - colliderA->position;
				if (std::abs(delta.x) < (colliderA->size + colliderB->size) &&
					std::abs(delta.y) < (colliderA->size + colliderB->size)) {
					// Handle collision response here if needed
				}
			}

			// Circle vs Box
			if ((colliderA->type == Collider::EColliderType::Circle && colliderB->type == Collider::EColliderType::Box) ||
				(colliderA->type == Collider::EColliderType::Box && colliderB->type == Collider::EColliderType::Circle)) {
				Collider* circleCollider = (colliderA->type == Collider::EColliderType::Circle) ? colliderA : colliderB;
				Collider* boxCollider = (colliderA->type == Collider::EColliderType::Box) ? colliderA : colliderB;
				olc::vf2d delta = circleCollider->position - boxCollider->position;
				olc::vf2d halfSize = { boxCollider->size, boxCollider->size };
				olc::vf2d closestPoint = delta.clamp(-halfSize, halfSize);
				olc::vf2d difference = delta - closestPoint;
				float distanceSquared = difference.mag2();
				if (distanceSquared < circleCollider->size * circleCollider->size) {
					// Handle collision response here if needed
				}
			}
		}
	}

	// apply pending bullet removals after collision pass (safe: not modifying colliders during iteration)
	if (!pendingBulletRemovals.empty()) {
		for (auto* b : pendingBulletRemovals) {
			if (!b) continue;
			if (b->collider) {
				UnregisterCollider(b->collider);
				// mark for removal on the bullet itself to let owner erase safely
				b->markedForRemoval = true;
			}
		}
		pendingBulletRemovals.clear();
	}
}