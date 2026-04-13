#pragma once
#include "Character.h"
#include "Player.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

class Bullet : public Character
{
public:
    void SetDirection(const olc::vf2d& dir)
    {
        direction = dir;
    }

    void Update(float elapsedTime) override
    {
        Position += direction * speed * elapsedTime;
    }

private:
    olc::vf2d direction;
    float speed = 200.0f;
};