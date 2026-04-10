#pragma once
#include "../AcademiaEngine-Study/src/Game/GameObject.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

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