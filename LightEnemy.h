#pragma once
#include "Ennemies.h"

class LightEnemy : public Ennemies
{
public:
    LightEnemy(olc::vf2d pos)
        : Ennemies(pos, 12.0f)
    {
        Health = 20.0f;
        Color = olc::YELLOW;
    }

    float GetSpeed() const override { return 120.0f; }
    float GetMass()  const override { return 0.6f; }
};