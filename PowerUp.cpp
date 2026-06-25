#include "PowerUp.h"

void PowerUp::Draw(AcademiaEngine& engine) {
    olc::Sprite* sheet = GetSprite(engine);
    if (!sheet) return;

    int frameWidth = 64;
    int frameHeight = 64;

    int x = Frame * frameWidth;
    int y = 0;

    const olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);

    int scale = GetScale();

    engine.DrawPartialSprite(
        pixelPos.x - (frameWidth * scale) / 2,
        pixelPos.y - (frameHeight * scale) / 2,
        sheet,
        x, y,
        frameWidth, frameHeight, scale
    );
}

void PowerUp::Update(AcademiaEngine& engine, float elapsedTime) {
	
}