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
    void AddForce(AcademiaEngine& engine, float force, olc::vf2d direction, float elapsedTime);
    void Update(AcademiaEngine& engine, float elapsedTime);

    const olc::vf2d& GetPlayerPosition(AcademiaEngine& engine) const;
    void GoToPlayer(AcademiaEngine& engine, float force, olc::vf2d playerPosition, float elapsedTime);

    const Player* GetPlayer() const { return player; }
    void SetPlayer(const Player* p){ player = p; }

protected:
    float Radius = 20.0f;
    olc::Pixel Color = olc::WHITE;

private:
    const Player* player = nullptr;
};