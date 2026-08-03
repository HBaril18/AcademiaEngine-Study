#pragma once
#include "Collider.h"
#include <vector>
#include <deque>
#include <memory>
#include <unordered_set>
#include <unordered_map>

// Forward declarations to avoid circular includes
class Player;
class Ennemies;
class Bullet;
class PowerUp;
class EnemyBullet;

/*======================NEW=============================*/
struct GridCell
{
	std::vector<Collider*> colliders;
};

struct CellKey
{
	int x;
	int y;

	bool operator==(const CellKey& other) const
	{
		return x == other.x &&
			y == other.y;
	}
};

struct CellKeyHash
{
	size_t operator()(const CellKey& key) const
	{
		return std::hash<int>()(key.x)
			^ (std::hash<int>()(key.y) << 1);
	}
};

/*======================================================*/


/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-06-03 //
//                                  //
/*----------------------------------*/

class CollisionManager
{
public:

	const bool collisionMatrix[5][5] = {
		// 0     1      2      3      4
		{false, true, false, false, false }, // 0 PowerUp
		{true, false, true,  false, true  }, // 1 Player
		{false, true,  false, true, false }, // 2 Enemy
		{false, false, true,  false, false}, // 3 Bullet
		{false, true, false, false, false }  // 4 Enemy bullet
	};
	/*=====================NEW=======================*/
	static constexpr float CELL_SIZE = 128.0f;

	std::unordered_map<
		CellKey,
		std::vector<Collider*>,
		CellKeyHash
	> grid;
	CellKey GetCell(
		const olc::vf2d& pos);
	void BuildGrid();
	std::vector<Collider*> GetNearbyColliders(
		const olc::vf2d& position);
	/*===============================================*/

	void RegisterCollider(Collider* collider);
	void UnregisterCollider(Collider* collider);
	void Update(float elapsedTime);
	void SetPlayer(Player* player) { this->player = player; }
	void SetEnnemies(std::deque<std::unique_ptr<Ennemies>>* ennemies) { this->ennemies = ennemies; }
	void SetBullets(std::list<Bullet>* bullets) { this->bullets = bullets; }
	void SetPowerUp(std::deque<std::unique_ptr<PowerUp>>* powerUp) { this->powerUp = powerUp; }
	// Return a list of pointers to all ennemies known to the manager (from explicit container or from colliders)
	std::vector<Ennemies*> GetEnnemies();
	// return the registered player or infer from colliders (layer == 1)
	Player* GetPlayer();
	// return list of bullets managed or inferred from colliders (layer == 3)
	std::vector<Bullet*> GetBullets();
	// return list of PowerUp managed or inferred from colliders (layer == 0)
	std::vector<PowerUp*> GetPowerUp();

	void CheckCollision(
		Collider* colliderA,
		Collider* colliderB,
		float elapsedTime);
	void CheckCellAgainstCell(
		const std::vector<Collider*>& a,
		const std::vector<Collider*>& b,
		float elapsedTime);

	// schedule a bullet to be removed (safe during update)
	void RemoveBullet(Bullet* b);

	void RemoveColliderPowerUp(PowerUp* p);

protected:
	std::vector<Collider*> colliders;

private:
	Player* player = nullptr;
	std::deque<std::unique_ptr<Ennemies>>* ennemies = nullptr;
	std::list<Bullet>* bullets = nullptr;
	std::deque<std::unique_ptr<PowerUp>>* powerUp = nullptr;
	std::vector<PowerUp*> pendingPowerUpRemovals;
};