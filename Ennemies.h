#pragma once
#include "Character.h"
#include "Player.h"
#include "Collider.h"
#include "CollisionManager.h"

// Forward declare CollisionManager to avoid including its header here
class CollisionManager;

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

// Plural feel odd here. Singular would be better :) 
class Ennemies : public Character
{
public:
    Ennemies();
    Ennemies(olc::vf2d pos, float radius);

    ~Ennemies();

    Ennemies(const Ennemies&) = delete;
    Ennemies& operator=(const Ennemies&) = delete;
    Ennemies(Ennemies&&) noexcept = default;
    Ennemies& operator=(Ennemies&&) noexcept = default;

    // Collision control
    void InitializeCollision(class CollisionManager* collisionManager);
    void ShutdownCollision(class CollisionManager* collisionManager);

    CollisionManager* collisionManager = nullptr;
    std::unique_ptr<Collider> collider = nullptr;
    olc::vf2d direction;

    void Draw(AcademiaEngine& engine) override;
    void AddForce(AcademiaEngine& engine, float force, olc::vf2d direction, float elapsedTime);
    void Update(AcademiaEngine& engine, float elapsedTime);

    void TakeDamage(float damage);
    static void RemoveEnnemie(std::deque<std::unique_ptr<Ennemies>>& ennemies);

    const olc::vf2d& GetPlayerPosition(AcademiaEngine& engine) const;
    void GetDirection(AcademiaEngine& engine, olc::vf2d playerPosition);

    const Player* GetPlayer() const { return player; }
    void SetPlayer(const Player* p);

    float GetRadius() const { return Radius; }

	float GetHealth() const { return Health; }

protected:
    float Radius = 15.0f;
    olc::Pixel Color = olc::DARK_BLUE;
	float Health = 50.0f;

private:
    const Player* player = nullptr;
};