#pragma once
#include "Character.h"
#include "Player.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

// Plural feel odd here. Singular would be better :) 
class Ennemies : public Character
{
public:
    void Draw(AcademiaEngine& engine) override;
    void AddForce(AcademiaEngine& engine, float force, olc::vf2d direction);
    
    // Pass the Player as a const Player& to document that the Player will not be modified here. 
    // GetPlayerPosition can be made const too, as it won't change your Ennemies instance.
    const olc::vf2d& GetPlayerPosition(AcademiaEngine& engine, const Player& player) const;
    void GoToPlayer(AcademiaEngine& engine, float force, olc::vf2d playerPosition);

protected:
    float Radius = 20.0f;
    olc::Pixel Color = olc::WHITE;
};