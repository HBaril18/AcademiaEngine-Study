#pragma once
#include "../AcademiaEngine-Study/src/Game/GameObject.h"

class Character : public GameObject
{
public:
    virtual ~Character() = default;

    virtual void Update(float elapsedTime) {}
    virtual void Draw(AcademiaEngine& engine) {}
    virtual void AddForce(AcademiaEngine& engine) {}

    float Radius;
    olc::Pixel Color;
};