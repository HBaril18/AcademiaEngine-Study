#pragma once
#include "Collider.h"
#include <vector>
#include <deque>

// Forward declarations to avoid circular includes
class Player;
class Ennemies;
class Bullet;

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
	void Update();
	void SetPlayer(Player* player) { this->player = player; }
	void SetEnnemies(std::deque<Ennemies>* ennemies) { this->ennemies = ennemies; }
	void SetBullets(std::deque<Bullet>* bullets) { this->bullets = bullets; }

protected:
	std::vector<Collider*> colliders;

private:
	Player* player = nullptr;
	std::deque<Ennemies>* ennemies = nullptr;
	std::deque<Bullet>* bullets = nullptr;
};