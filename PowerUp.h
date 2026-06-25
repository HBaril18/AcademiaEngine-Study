#pragma once
#include "src/Game/GameObject.h"

class PowerUp : public GameObject
{
public:
    PowerUp(olc::vf2d pos, float radius, );
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

private:
};