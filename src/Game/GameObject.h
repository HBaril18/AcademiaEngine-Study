#pragma once
#include "../../external/olc/olcPixelGameEngine.h"

class AcademiaEngine;

class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void Update(float elapsedTime) {}
    virtual void Draw(AcademiaEngine& engine) {}

    olc::vf2d Position;
};
