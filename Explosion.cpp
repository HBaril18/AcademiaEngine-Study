#include "Explosion.h"

void Explosion::Update(float fElapsedTime){
    timer += fElapsedTime;

    if (timer >= frameDuration)
    {
        timer = 0.0f;
        currentFrame++;

        if (currentFrame >= explosionFrames.size())
            finished = true;
    }
}

void Explosion::Draw(olc::PixelGameEngine* pge) {
    {
        if (finished) return;

        auto& frame = explosionFrames[currentFrame];

        for (auto& offset : frame.pixels)
        {
            pge->Draw(position + offset, frame.color);
        }
    }
}