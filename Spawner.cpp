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

	// set shared collision manager and register collider if available
	if (collisionManager) {
		ennemie.collisionManager = collisionManager;
		if (ennemie.collider) collisionManager->RegisterCollider(ennemie.collider);
	}

	std::cout << "Ennemie deque" << ennemies.size() << std::endl;
}
