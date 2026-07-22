#include "Explosion.h"
#include <cmath>
#include <random>

Explosion::Explosion(olc::vf2d pos, ExplosionType type)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> spread(-5.0f, 5.0f);
    std::uniform_real_distribution<float> angle(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> speed(50.0f, 150.0f);
    std::uniform_real_distribution<float> lifeDist(0.8f, 2.0f);

    for (int i = 0; i < 150; i++)
    {
        float a = angle(rng);
        float s = speed(rng);

        Particle p;
        p.pos = pos + olc::vf2d(spread(rng), spread(rng));
        p.vel = { cos(a) * s, sin(a) * s * 0.5f };
        p.life = lifeDist(rng);

        if (type == ExplosionType::Enemy)
        {
            if (i % 3 == 0) p.color = olc::YELLOW;
            else if (i % 3 == 1) p.color = olc::RED;
            else p.color = olc::DARK_RED;
        }
        else if (type == ExplosionType::Player)
        {
            if (i % 3 == 0) p.color = olc::CYAN;
            else if (i % 3 == 1) p.color = olc::BLUE;
            else p.color = olc::WHITE;
        }

        particles.push_back(p);
    }
}

void Explosion::Update(float dt)
{
    finished = true;

    for (auto& p : particles)
    {
        if (p.life <= 0) continue;

        // Gravité (fait tomber les morceaux)
        p.vel.y -= 150.0f * dt;

        // Mouvement
        p.pos += p.vel * dt;

        // Friction (ralentit)
        p.vel *= 0.98f;

        // Réduction de vie
        p.life -= dt;

        if (p.life > 0)
            finished = false;
    }
}

void Explosion::Draw(AcademiaEngine* engine)
{
    for (auto& p : particles)
    {
        if (p.life <= 0) continue;

        auto screenPos = engine->ConvertWorldPositionToPixels(p.pos);

        // Dessine un pixel (ou petit carré)
        engine->FillRect(screenPos, { 3,3 }, p.color);
    }
}
