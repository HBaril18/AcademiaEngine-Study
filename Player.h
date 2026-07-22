#pragma once
#include "Character.h"
#include "Bullet.h"
#include "PowerUpType.h"
#include <vector>
#include <deque>

// Forward declaration
class GameManager;
class CollisionManager;
struct Collider;

struct ActivePowerUp
{
	PowerUpType type;
	float remainingTime;
};

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

// Moving all those files in the Game directory would make it easier to navigate. (Player - Ennemies - Bullet)
class Player : public Character
{
public:
	Player(AcademiaEngine& engine);
	virtual ~Player();

	float speedPowerupTimer = 0.0f;
	float damagePowerupTimer = 0.0f;
	float shieldPowerupTimer = 0.0f;

	float damageCooldown = 0.0f;     // temps restant
	float damageDelay = 0.45f;        // délai entre dégâts (en secondes)
	bool hasExploded = false;
	olc::vf2d knockbackVelocity = { 0.0f, 0.0f };
	olc::Sprite* sprite = nullptr;
	olc::Decal* decal = nullptr;

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
	void SetGameManager(GameManager* gm) { gameManager = gm; }
	void AddScore(float scoreToAdd);
	virtual olc::Sprite* GetSprite(AcademiaEngine& engine)
	{
		return &engine.PlayerSheet;
	}
	void SetSpeedMultiplier(float s) { SpeedMultiplier = s; }
	void SetDamageMultiplier(float d) { DamageMultiplier = d; }
	float GetSpeedMultiplier() const { return SpeedMultiplier; }
	float GetDamageMultiplier() const { return DamageMultiplier; }
	void AddShield() { Shield += 1; }
	void RemoveShield() { if (Shield > 0) Shield -= 1; }
	int GetShield() { return Shield; }
	void ActivateSpeedPowerup();
	void ActivateDamagePowerup();
	void ActivateShieldPowerup();

	// Collision
	Collider* collider = nullptr;

	// Initialize and shutdown collision for this player using the given manager
	void InitializeCollision(class CollisionManager* collisionManager);
	void ShutdownCollision(class CollisionManager* collisionManager);
	// stored pointer to the collision manager
	CollisionManager* collisionManager = nullptr;

protected:
	float SpeedMultiplier = 1.0f;
	float DamageMultiplier = 1.0f;
	int Shield = 0;
    float Radius = 20.0f;
	olc::Pixel Color = olc::Pixel(46, 230, 214);
    std::deque<Bullet> bullets;
	float Health = 100.0f;
	float Damage = 10.0f;
	GameManager* gameManager = nullptr;
	bool IsAlive() const { return Health > 0.0f; }
	float Scale = 0.5f;
private:
	olc::Sprite* spriteSheet = nullptr;
	AcademiaEngine* _Engine = nullptr;
	std::vector<ActivePowerUp> _ActivePowerUps;
};