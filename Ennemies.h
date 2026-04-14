#pragma once
#include "Character.h"
#include "Player.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

class Ennemies : public Character
{
public:
    void Draw(AcademiaEngine& engine) override;
    void AddForce(AcademiaEngine& engine, float force, olc::vf2d direction);
    olc::vf2d GetPlayerPosition(AcademiaEngine& engine, Player& player);
    void GoToPlayer(AcademiaEngine& engine, float force, olc::vf2d playerPosition);

    float Radius = 20.0f;
    olc::Pixel Color = olc::WHITE;
};