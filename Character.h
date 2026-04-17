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
    // There is no need to redefine empty virtual functions (Update, Draw) in a derived class.
    // However, the virtual destructor is a special case: if you ever delete a Character* that
    // actually points to a derived object (Player, Ennemies...), c++ will only call ~Character()
    // and skip the derived destructor — which is undefined behavior.
    virtual ~Character() = default;

    //virtual void Update(float elapsedTime) {}
    //virtual void Draw(AcademiaEngine& engine) {}

    virtual void AddForce(AcademiaEngine& engine) {}

    float Radius;
    olc::Pixel Color;
};