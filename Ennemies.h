#pragma once
#include "Character.h"
#include "Player.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "src/Game/GameManager.h"

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

    virtual ~Ennemies();

    // --- Fonctions virtuelles ---
    virtual void Update(AcademiaEngine& engine, float elapsedTime);
    virtual void Draw(AcademiaEngine& engine) override;
    virtual void TakeDamage(float damage, float elapsedTime);

    // --- Stats virtuelles ---
    virtual float GetSpeed() const { return 60.0f; }
    virtual float GetDamage() const { return 10.0f; }
    virtual float GetMass()   const { return 1.0f; }

    void InitializeCollision(class CollisionManager* collisionManager);
    void ShutdownCollision(class CollisionManager* collisionManager);

    void AddForce(AcademiaEngine& engine, float force, olc::vf2d direction, float elapsedTime);

    static void RemoveEnnemie(std::deque<std::unique_ptr<Ennemies>>& ennemies);

    const olc::vf2d& GetPlayerPosition(AcademiaEngine& engine) const;
    void GetDirection(AcademiaEngine& engine, olc::vf2d playerPosition);
    Player* GetPlayer() const { return player; }
    void SetPlayer(Player* p);

    float GetRadius() const { return Radius; }
    float GetHealth() const { return Health; }

    bool hasExploded = false;
    GameManager* gameManager = nullptr;
    CollisionManager* collisionManager = nullptr;
    std::unique_ptr<Collider> collider = nullptr;
    olc::vf2d direction;
    olc::vf2d recoilVelocity;
protected:
    float Radius = 15.0f;
    olc::Pixel Color = olc::Pixel(255, 59, 59);
    float Health = 50.0f;

private:
    Player* player = nullptr;
};