#pragma once
#include "external/olc/olcPixelGameEngine.h"
#include "src/Engine/AcademiaEngine.h"
#include <vector>

enum class ExplosionType
{
    Enemy,
    Player
};

struct Particle
{
    olc::vf2d pos;
    olc::vf2d vel;
    float life = 0.0f;
    olc::Pixel color;
};

class Explosion
{
public:
    std::vector<Particle> particles;
    bool finished = false;

    Explosion(olc::vf2d pos, ExplosionType type);

    void Update(float dt);
    void Draw(AcademiaEngine* engine);
};