#pragma once
#include "Character.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

class Bullet : public Character
{
public:
    void SetDirection(const olc::vf2d& dir);
	void Update(float elapsedTime) override;
    void Draw(AcademiaEngine& engine) override;

private:
    olc::vf2d direction;
    olc::Pixel Color = olc::DARK_YELLOW;
    float speed = 1200.0f;
	float radius = 5.0f;
};