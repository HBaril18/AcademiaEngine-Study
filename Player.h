#pragma once
#include "Character.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

class Player : public Character
{
public:
    void Draw(AcademiaEngine& engine) override;
	void DrawCursor(AcademiaEngine& engine, olc::vf2d cursorWorldPos);
    void AddForce(AcademiaEngine& engine, float force, std::vector<float> direction);
    olc::vf2d GetCursorPosition(AcademiaEngine& engine);
	olc::vf2d GetPlayerDirection(AcademiaEngine& engine);

    float Radius = 20.0f;
    olc::Pixel Color = olc::WHITE;
};
