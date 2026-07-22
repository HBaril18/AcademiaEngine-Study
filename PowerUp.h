#pragma once
#include "Character.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "src/Game/GameManager.h"
#include "Player.h"
#include "PowerUpType.h"
#include <sstream>
#include <iomanip>

class Player;

class PowerUp : public GameObject
{
public:
    float temp = 0.0f;
    bool collected = false;
    GameManager* gameManager = nullptr;
    std::unique_ptr<Collider> collider = nullptr;
    CollisionManager* collisionManager = nullptr;

    void InitializeCollision(class CollisionManager* collisionManager);
    void ShutdownCollision(class CollisionManager* collisionManager);

    PowerUp(olc::vf2d pos, float radius, PowerUpType type);
	void Update(AcademiaEngine& engine, float elapsedTime) override;
	void Draw(AcademiaEngine& engine) override;
    float GetScale() { return Scale; }
    PowerUpType GetType() { return type; }
    constexpr std::string_view GetTypeName(PowerUpType pu) {
        switch (pu) {
        case PowerUpType::Heal: return "Heal";
        case PowerUpType::Damage: return "Damage";
        case PowerUpType::Speed: return "Speed";
        case PowerUpType::Shield: return "Shield";
        default: return "Unknown";
        }
    }
    olc::Sprite* GetSprite(AcademiaEngine& engine)
    {
        return &engine.PowerUpSheet;
    }
    void SetGameManager(GameManager* gm) { gameManager = gm; }
    void SetSprite(olc::Sprite* sprite)
    {
        spriteSheet = sprite;
    }
    void Apply(Player& player);
    olc::Sprite* sprite = nullptr;
    olc::Decal* decal = nullptr;
    bool markedForRemoval = false;

protected:
    float Radius = 64.0f;
    int Frame = 0;
    float _animationTimer = 0.0f;
    float Scale = 1;
    PowerUpType type;


private:
    olc::Sprite* spriteSheet = nullptr;
};