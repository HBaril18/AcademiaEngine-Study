#pragma once
#include "GameObject.h"

class ExampleGameObject : public GameObject
{
public:
    void Draw(AcademiaEngine& engine) override;

    float Radius = 20.0f;
    olc::Pixel Color = olc::WHITE;
};
