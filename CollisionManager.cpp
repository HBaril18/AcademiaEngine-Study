#include "CollisionManager.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Player.h"
#include "Ennemies.h"
#include "Bullet.h"
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

void CollisionManager::Update()
{
	// Implementation for updating collisions
	for (int i = 0; i < static_cast<int>(colliders.size()); ++i) {
		for (int j = i + 1; j < static_cast<int>(colliders.size()); ++j) {
			Collider* colliderA = colliders[i];
			Collider* colliderB = colliders[j];
			if (!colliderA || !colliderB) continue;
			// Check if both colliders are enabled
			if (!colliderA->enabled || !colliderB->enabled) continue;
			if (colliderA->owner == nullptr || colliderB->owner == nullptr) continue;
			if (!player) continue; // player must be set to handle player collision

			// layer bounds check
			if (colliderA->layer < 0 || colliderA->layer >= 4 || colliderB->layer < 0 || colliderB->layer >= 4) continue;

			if (colliderA->enabled && colliderB->enabled) {
				const bool collisionMatrix[4][4] = {
					// 0    1      2      3
					{false, false, false, false}, // 0 unused
					{false, false, true,  false}, // 1 Player
					{false, true,  false, true }, // 2 Enemy
					{false, false, true,  false}  // 3 Bullet
				};

				int a = colliderA->layer;
				int b = colliderB->layer;
				//std::cout << "Checking collision between layer " << a << " and layer " << b << std::endl;
				//std::cout << "collision is true ? " << collisionMatrix[a][b] << std::endl;
				if (collisionMatrix[a][b]) {
					// Collision logic
					if (colliderA->type == Collider::EColliderType::Circle && colliderB->type == Collider::EColliderType::Circle) {
						// Circle vs Circle collision logic
						olc::vf2d delta = colliderB->position - colliderA->position;
						float distanceSquared = delta.mag2();
						float radiusSum = colliderA->size + colliderB->size;
						// debug
						// std::cout << "Circle-Circle pair: a="<<a<<" b="<<b<<" dist2="<<distanceSquared<<" rsum2="<<radiusSum*radiusSum<<"\n";
						if (distanceSquared < radiusSum * radiusSum) {
							// Handle collision response here
							// Determine pair by layer a and b
							// Player-Enemy
							if ((a == 1 && b == 2) || (a == 2 && b == 1)) {
								// damage the player
								if (player && player->damageCooldown <= 0.0f) {
									player->TakeDamage(10.0f);
									player->damageCooldown = player->damageDelay;
								}
							}
							// Enemy-Bullet
							else if ((a == 2 && b == 3) || (a == 3 && b == 2)) {
								Bullet* victim = nullptr;
								if (a == 2) {
									auto* e = static_cast<Ennemies*>(colliderA->owner);
									if (e) {
										e->TakeDamage(25.0f);
										// bullet is colliderB
										victim = static_cast<Bullet*>(colliderB->owner);
									}
									else {
										auto* e = static_cast<Ennemies*>(colliderB->owner);
										if (e) {
											e->TakeDamage(25.0f);
											// bullet is colliderA
											victim = static_cast<Bullet*>(colliderA->owner);
										}
										if (victim) RemoveBullet(victim);
										// Optionally, remove or mark bullet for deletion here
									}
									// Other collision responses can be added here
								}
							}
							if (colliderA->type == Collider::EColliderType::Box && colliderB->type == Collider::EColliderType::Box) {
								// Box vs Box collision logic
								olc::vf2d delta = colliderB->position - colliderA->position;
								if (std::abs(delta.x) < (colliderA->size + colliderB->size) &&
									std::abs(delta.y) < (colliderA->size + colliderB->size)) {
									// Handle collision response here

								}
							}
							if ((colliderA->type == Collider::EColliderType::Circle && colliderB->type == Collider::EColliderType::Box) ||
								(colliderA->type == Collider::EColliderType::Box && colliderB->type == Collider::EColliderType::Circle)) {
								// Circle vs Box collision logic
								Collider* circleCollider = (colliderA->type == Collider::EColliderType::Circle) ? colliderA : colliderB;
								Collider* boxCollider = (colliderA->type == Collider::EColliderType::Box) ? colliderA : colliderB;
								olc::vf2d delta = circleCollider->position - boxCollider->position;
								olc::vf2d halfSize = { boxCollider->size, boxCollider->size };
								olc::vf2d closestPoint = delta.clamp(-halfSize, halfSize); //Only for square box, for rectangle we would need to have different halfSize for x and y
								olc::vf2d difference = delta - closestPoint;
								float distanceSquared = difference.mag2();
								if (distanceSquared < circleCollider->size * circleCollider->size) {
									// Handle collision response here

								}
							}
						}
					}
				}
				// apply pending bullet removals after collision pass
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
		}
	}
}