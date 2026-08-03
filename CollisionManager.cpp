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
		if (c && c->owner && c->layer == 0) {
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
	BuildGrid();

	for (auto& [key, cell] : grid)
	{
		auto& cellColliders = cell;

		// Check pairs inside the same cell
		for (size_t i = 0; i < cellColliders.size(); i++)
		{
			for (size_t j = i + 1; j < cellColliders.size(); j++)
			{
				CheckCollision(
					cellColliders[i],
					cellColliders[j],
					elapsedTime);
			}
		}

		// Check right cell
		CellKey right{ key.x + 1, key.y };
		auto it = grid.find(right);

		if (it != grid.end())
		{
			CheckCellAgainstCell(
				cellColliders,
				it->second,
				elapsedTime);
		}

		// Check bottom cell
		CellKey bottom{ key.x, key.y + 1 };
		it = grid.find(bottom);

		if (it != grid.end())
		{
			CheckCellAgainstCell(
				cellColliders,
				it->second,
				elapsedTime);
		}

		// Check bottom-right cell
		CellKey bottomRight{ key.x + 1, key.y + 1 };
		it = grid.find(bottomRight);

		if (it != grid.end())
		{
			CheckCellAgainstCell(
				cellColliders,
				it->second,
				elapsedTime);
		}

		// Check bottom-left cell
		CellKey bottomLeft{ key.x - 1, key.y + 1 };
		it = grid.find(bottomLeft);

		if (it != grid.end())
		{
			CheckCellAgainstCell(
				cellColliders,
				it->second,
				elapsedTime);
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

void CollisionManager::CheckCollision(
	Collider* colliderA,
	Collider* colliderB,
	float elapsedTime)
{
	if (!colliderA || !colliderB)
		return;

	if (colliderA == colliderB)
		return;

	if (!colliderA->enabled || !colliderB->enabled)
		return;

	if (!colliderA->owner || !colliderB->owner)
		return;

	if (!player)
		return;

	if (colliderA->layer < 0 || colliderA->layer >= 5)
		return;

	if (colliderB->layer < 0 || colliderB->layer >= 5)
		return;

	int a = colliderA->layer;
	int b = colliderB->layer;

	if (!collisionMatrix[a][b])
		return;

	if (colliderA->type != Collider::EColliderType::Circle ||
		colliderB->type != Collider::EColliderType::Circle)
	{
		return;
	}

	olc::vf2d delta = colliderB->position - colliderA->position;
	float distanceSquared = delta.mag2();

	float radiusSum = colliderA->size + colliderB->size;
	bool overlap = distanceSquared < radiusSum * radiusSum;

	// Sweep test for fast player bullets vs enemies
	if (!overlap)
	{
		bool isEnemyBullet =
			(a == 2 && b == 3) ||
			(a == 3 && b == 2);

		if (isEnemyBullet)
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
				return;

			if (bullet->markedForRemoval ||
				!bullet->collider ||
				!bullet->collider->enabled)
			{
				return;
			}

			olc::vf2d p1 = bullet->GetPreviousPosition();
			olc::vf2d p2 = bullet->GetPosition();
			olc::vf2d c = enemy->GetPosition();
			olc::vf2d d = p2 - p1;

			float len2 = d.mag2();

			if (len2 > 0.0f)
			{
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

	if (!overlap)
		return;

	// Player-Enemy
	if ((a == 1 && b == 2) || (a == 2 && b == 1))
	{
		Player* p = player;
		Ennemies* e = nullptr;

		if (a == 2)
			e = static_cast<Ennemies*>(colliderA->owner);
		else
			e = static_cast<Ennemies*>(colliderB->owner);

		if (p && e && p->damageCooldown <= 0.0f)
		{
			if (p->GetShield() > 0)
			{
				p->RemoveShield();
			}
			else
			{
				p->TakeDamage(10.0f);
			}

			p->damageCooldown = p->damageDelay;

			olc::vf2d dir = p->GetPosition() - e->GetPosition();

			if (dir.mag2() > 0.001f)
				dir = dir.norm();

			float knockbackStrength = 550.0f;

			p->knockbackVelocity += dir * knockbackStrength;
			e->recoilVelocity -= dir * 300.0f;
		}

		return;
	}

	// Enemy-Bullet
	if ((a == 2 && b == 3) || (a == 3 && b == 2))
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
			return;

		if (bullet->markedForRemoval ||
			!bullet->collider ||
			!bullet->collider->enabled)
		{
			return;
		}

		enemy->TakeDamage(
			10.0f * player->GetDamageMultiplier(),
			elapsedTime);

		RemoveBullet(bullet);

		return;
	}

	// EnemyBullet-Player
	if ((a == 4 && b == 1) || (a == 1 && b == 4))
	{
		EnemyBullet* enemyBullet = nullptr;
		Player* p = nullptr;

		if (a == 4)
		{
			enemyBullet = static_cast<EnemyBullet*>(colliderA->owner);
			p = static_cast<Player*>(colliderB->owner);
		}
		else
		{
			enemyBullet = static_cast<EnemyBullet*>(colliderB->owner);
			p = static_cast<Player*>(colliderA->owner);
		}

		if (!enemyBullet || !p)
			return;

		if (enemyBullet->markedForRemoval ||
			!enemyBullet->collider ||
			!enemyBullet->collider->enabled)
		{
			return;
		}

		if (p->damageCooldown <= 0.0f)
		{
			if (p->GetShield() > 0)
			{
				p->RemoveShield();
			}
			else
			{
				p->TakeDamage(enemyBullet->damage);
			}

			p->damageCooldown = p->damageDelay;
		}

		enemyBullet->markedForRemoval = true;

		if (enemyBullet->collider)
			enemyBullet->collider->enabled = false;

		return;
	}

	// Player-PowerUp
	if ((a == 1 && b == 0) || (a == 0 && b == 1))
	{
		Player* p = nullptr;
		PowerUp* pu = nullptr;

		if (a == 1)
		{
			p = static_cast<Player*>(colliderA->owner);
			pu = static_cast<PowerUp*>(colliderB->owner);
		}
		else
		{
			pu = static_cast<PowerUp*>(colliderA->owner);
			p = static_cast<Player*>(colliderB->owner);
		}

		if (pu && p && !pu->collected)
		{
			pu->Apply(*p);
			pu->collected = true;
			pu->markedForRemoval = true;
			RemoveColliderPowerUp(pu);
		}

		return;
	}
}

void CollisionManager::CheckCellAgainstCell(
	const std::vector<Collider*>& a,
	const std::vector<Collider*>& b,
	float elapsedTime)
{
	for (Collider* c1 : a)
	{
		for (Collider* c2 : b)
		{
			CheckCollision(
				c1,
				c2,
				elapsedTime);
		}
	}
}
