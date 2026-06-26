#include "PowerUp.h"

PowerUp::PowerUp(olc::vf2d pos, float radius, PowerUpType t) {
    Position = pos;
    Radius = radius;
    type = t;

    collider = std::make_unique<Collider>();

    collider->owner = this;
    collider->position = Position;
    collider->size = Radius;
    collider->layer = 0;
    collider->type = Collider::EColliderType::Circle;
    collider->enabled = true;

    if (collisionManager) {
        collisionManager->RegisterCollider(collider.get());
    }
}

void PowerUp::Draw(AcademiaEngine& engine) {
    olc::Sprite* sheet = GetSprite(engine);
    if (!sheet) return;

    int frameWidth = Radius;
    int frameHeight = Radius;

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