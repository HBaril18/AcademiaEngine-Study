#pragma once
#include "external/olc/olcPixelGameEngine.h"

struct ExplosionFrame
{
    std::vector<olc::vi2d> pixels; // positions relatives
    olc::Pixel color;
};

class Explosion
{
public:
    olc::vi2d position;
    float timer = 0.0f;
    float frameDuration = 0.1f;
    int currentFrame = 0;
    bool finished = false;

    std::vector<ExplosionFrame> explosionFrames =
    {
        { { {0,0} }, olc::YELLOW }, // centre

        { { {-1,0}, {1,0}, {0,-1}, {0,1} }, olc::YELLOW },

        { { {-2,0}, {2,0}, {0,-2}, {0,2},
            {-1,-1}, {1,-1}, {-1,1}, {1,1} }, olc::RED },

        { { {-3,0}, {3,0}, {0,-3}, {0,3},
            {-2,-2}, {2,-2}, {-2,2}, {2,2} }, olc::DARK_RED }
    };

    Explosion(olc::vi2d pos) : position(pos) {}

    void Update(float fElapsedTime);

    void Draw(olc::PixelGameEngine* pge);
};