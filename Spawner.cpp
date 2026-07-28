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

#include <random>

Spawner::Spawner()
{
    SpawnInterval = 1.0f;
    SpawnTimer = SpawnInterval;
}

void Spawner::Update(AcademiaEngine& engine, float elapsedTime)
{
    SpawnTimer += elapsedTime;

    if (SpawnTimer >= SpawnInterval)
    {
        SpawnTimer = 0.0f;

        if (gameManager)
        {
            gameManager->SpawnRandomEnemy();
        }

        static std::mt19937 rng(std::random_device{}());

        std::uniform_real_distribution<float>
            nextSpawn(1.0f, 6.0f);

        SpawnInterval = nextSpawn(rng);
    }
}