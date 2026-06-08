#pragma once
#include "Character.h"
#include "Bullet.h"
#include <vector>
#include <deque>

// Forward declaration
class CollisionManager;
struct Collider;

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

// Moving all those files in the Game directory would make it easier to navigate. (Player - Ennemies - Bullet)
class Player : public Character
{
public:
	float damageCooldown = 0.0f;     // temps restant
	float damageDelay = 0.65f;        // délai entre dégâts (en secondes)

    void Draw(AcademiaEngine& engine) override;
	void DrawCursor(AcademiaEngine& engine, olc::vf2d cursorWorldPos);
	void AddForce(AcademiaEngine& engine, float force, const std::vector<float>& direction, float elapsedTime);
    olc::vf2d GetCursorPosition(AcademiaEngine& engine) const;
	olc::vf2d GetPlayerDirection(AcademiaEngine& engine) const;
	void SpawnBullet(AcademiaEngine& engine);
	std::deque<Bullet>& GetBullets() { return bullets; }
	void Update(AcademiaEngine& engine, float elapsedTime);
	void UpdateBullets(AcademiaEngine& engineContext);
	void TakeDamage(float damage) { Health = Health - damage; }
	float GetHealth() const { return Health; }
	float SetHealth(float health) { Health = health; return Health; }

	// Collision
	Collider* collider = nullptr;

	// Initialize and shutdown collision for this player using the given manager
	void InitializeCollision(class CollisionManager* collisionManager);
	void ShutdownCollision(class CollisionManager* collisionManager);
	// stored pointer to the collision manager
	CollisionManager* collisionManager = nullptr;

protected:
    float Radius = 20.0f;
	olc::Pixel Color = olc::Pixel(46, 230, 214);
    std::deque<Bullet> bullets;
	float Health = 100.0f;
	float Damage = 10.0f;
	bool IsAlive() const { return Health > 0.0f; }
};