#pragma once
#include "Character.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "src/Game/GameManager.h"

enum PowerUpType
{
    Heal,
    DoubleGun,
    AutoShooting,
    Rockets
};

class PowerUp : public GameObject
{
public:
    GameManager* gameManager = nullptr;
    std::unique_ptr<Collider> collider = nullptr;
    CollisionManager* collisionManager = nullptr;

    PowerUp(olc::vf2d pos, float radius, PowerUpType type);
	void Update(AcademiaEngine& engine, float elapsedTime) override;
	void Draw(AcademiaEngine& engine) override;
    int GetScale() { return Scale; }
    olc::Sprite* GetSprite(AcademiaEngine& engine)
    {
        return &engine.RedSlimeSheet;
    }


protected:
    float Radius = 64.0f;
    int Frame = 0;
    float Timer = 0.0f;
    float Scale = 1;
    PowerUpType type;

private:
};