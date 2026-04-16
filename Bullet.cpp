#include "Bullet.h"

void Bullet::Draw(AcademiaEngine& engine) {
    olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
    engine.FillCircle(pixelPos, static_cast<int32_t>(radius), Color);
}

void Bullet::Update(float elapsedTime)
{
    Position += direction * speed * elapsedTime;
}

void Bullet::SetDirection(const olc::vf2d& dir)
{
    direction = dir;
}