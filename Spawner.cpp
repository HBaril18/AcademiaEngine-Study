#include "Spawner.h"
#include "Ennemies.h"
#include <memory>
#include <iostream>
#include <mutex>
// ensure <memory> is available for std::unique_ptr usage

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

void Spawner::SpawnEnnemies(AcademiaEngine& engine, Player* player, CollisionManager* cm) {
	// Spawn at the spawner position and associate the player pointer
	auto enemy = std::make_unique<Ennemies>(Position, 15.0f);
	if (player) enemy->SetPlayer(player);
	enemy->InitializeCollision(cm);
	// Lock to avoid concurrent modification of the deque from other threads
	{
		std::lock_guard<std::mutex> lk(ennemies_mutex);
		ennemies_container.push_back(std::move(enemy));
	}
	std::cout << "Spawned enemy at (" << Position.x << "," << Position.y << ") player ptr=" << reinterpret_cast<const void*>(player) << "\n";
}