#pragma once
#include "../../external/olc/olcPixelGameEngine.h"

class AcademiaEngine;

class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void Update(float elapsedTime) {}
    virtual void Draw(AcademiaEngine& engine) {}
    
    const olc::vf2d& GetPosition() const {return Position;}
    void SetPosition(const olc::vf2d& position) {Position = position;}

protected:
    olc::vf2d Position;
};
