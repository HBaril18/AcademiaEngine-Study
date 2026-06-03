#include "Spawner.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

void Spawner::SpawnEnnemies(AcademiaEngine& engine, Player* player, CollisionManager* collisionManager) {
	// Construct enemy in-place in the deque using current Position and default radius
	ennemies.emplace_back(Position, 15.0f);
	Ennemies& ennemie = ennemies.back();

	// set player reference
	ennemie.SetPlayer(player);

	// initialize collision for the enemy using the shared manager
	if (collisionManager) {
		ennemie.InitializeCollision(collisionManager);
	}

	std::cout << "Ennemie deque" << ennemies.size() << std::endl;
}
