#include "Spawner.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

void Spawner::SpawnEnnemies(AcademiaEngine& engine, Player* player) {

	Ennemies ennemie = Ennemies(); // Create a new ennemie instance

	ennemie.SetPosition(Position); // Set the ennemie's initial position to the spawner's position 
	ennemie.SetPlayer(player); // Set reference to player

	ennemies.push_back(ennemie); // Add the enemy to the spawner's list
	std::cout << "Ennemie deque" << ennemies.size() << std::endl;
}
