#pragma once
#include "Character.h"

class Player : public Character
{
public:
    void Draw(AcademiaEngine& engine) override;
    void AddForce(AcademiaEngine& engine, float force, std::vector<float> direction) override;

    float Radius = 20.0f;
    olc::Pixel Color = olc::WHITE;
};
