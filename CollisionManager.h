#pragma once
#include "Collider.h"
#include <vector>
#include <deque>
#include <memory>

// Forward declarations to avoid circular includes
class Player;
class Ennemies;
class Bullet;
class PowerUp;

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-06-03 //
//                                  //
/*----------------------------------*/

class CollisionManager
{
public:
	void RegisterCollider(Collider* collider);
	void UnregisterCollider(Collider* collider);
	void Update(float elapsedTime);
	void SetPlayer(Player* player) { this->player = player; }
	void SetEnnemies(std::deque<std::unique_ptr<Ennemies>>* ennemies) { this->ennemies = ennemies; }
	void SetBullets(std::deque<Bullet>* bullets) { this->bullets = bullets; }
	void SetPowerUp(std::deque<std::unique_ptr<PowerUp>>* powerUp) { this->powerUp = powerUp; }
	// Return a list of pointers to all ennemies known to the manager (from explicit container or from colliders)
	std::vector<Ennemies*> GetEnnemies();
	// return the registered player or infer from colliders (layer == 1)
	Player* GetPlayer();
	// return list of bullets managed or inferred from colliders (layer == 3)
	std::vector<Bullet*> GetBullets();
	// return list of PowerUp managed or inferred from colliders (layer == 0)
	std::vector<PowerUp*> GetPowerUp();

	// schedule a bullet to be removed (safe during update)
	void RemoveBullet(Bullet* b);

protected:
	std::vector<Collider*> colliders;
	// pending removals for bullets to avoid modifying colliders during iteration
	std::vector<class Bullet*> pendingBulletRemovals;

private:
	Player* player = nullptr;
	std::deque<std::unique_ptr<Ennemies>>* ennemies = nullptr;
	std::deque<Bullet>* bullets = nullptr;
	std::deque<std::unique_ptr<PowerUp>>* powerUp = nullptr;
};