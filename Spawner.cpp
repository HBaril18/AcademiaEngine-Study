#include "Spawner.h"
#include "Ennemies.h"
#include "LightEnemy.h"
#include <memory>
#include <iostream>
#include <mutex>
// ensure <memory> is available for std::unique_ptr usage

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

void Spawner::SpawnEnnemies(AcademiaEngine& engine, Player* player, CollisionManager* cm)
{
    std::unique_ptr<Ennemies> enemy;

    int r = rand() % 2;

    if (r == 0)
        enemy = std::make_unique<Ennemies>(Position, 15.0f);
    else if (r == 1)
        enemy = std::make_unique<LightEnemy>(Position);

    if (player)
        enemy->SetPlayer(player);

    enemy->InitializeCollision(cm);

    {
        std::lock_guard<std::mutex> lk(ennemies_mutex);
        ennemies_container.push_back(std::move(enemy));
    }
}