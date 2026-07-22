#pragma once
#include "Ennemies.h"

class LightEnemy : public Ennemies
{
public:
    LightEnemy(olc::vf2d pos, float radius = 12.0f, float maxH = 20.0f)
        : Ennemies(pos, radius, maxH)
    {
        MaxHealth = maxH;
        Radius = radius;
        Health = MaxHealth;
        Color = olc::YELLOW;
        Scale = 1;
    }
    olc::Sprite* GetSprite(AcademiaEngine& engine) override
    {
        return &engine.YellowSlimeSheet;
    }


    float GetSpeed() const override { return 120.0f; }
    float GetMass()  const override { return 0.6f; }
    int GetScale() override { return Scale; }

protected:
    int Scale = 1;
};