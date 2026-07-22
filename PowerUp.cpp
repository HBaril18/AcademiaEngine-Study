#include "PowerUp.h"

PowerUp::PowerUp(olc::vf2d pos, float radius, PowerUpType t) {
    Position = pos;
    Radius = radius;
    type = t;
}

void PowerUp::InitializeCollision(CollisionManager* collisionManager)
{
    this->collisionManager = collisionManager;
    if (!collider) {
        collider = std::make_unique<Collider>();
    }
    // ensure collider fields are up-to-date
    collider->owner = this;
    collider->position = Position;
    collider->size = Radius;
    collider->layer = 0; // Enemy layer
    collider->type = Collider::EColliderType::Circle;
    collider->enabled = true;
    if (collisionManager) collisionManager->RegisterCollider(collider.get());
}

void PowerUp::ShutdownCollision(CollisionManager* collisionManager)
{
    if (collider) {
        if (collisionManager) collisionManager->UnregisterCollider(collider.get());
        collider.reset();
    }
    this->collisionManager = nullptr;
}

void PowerUp::Draw(AcademiaEngine& engine)
{
    olc::Sprite* sheet = GetSprite(engine);
    if (!sheet) return;

    constexpr int columns = 4;
    constexpr int rows = 4;

    int frameWidth = sheet->width / columns;
    int frameHeight = sheet->height / rows;

    int frameX = Frame % columns;
    int frameY = Frame / columns;

    int x = frameX * frameWidth;
    int y = frameY * frameHeight;

    const olc::vi2d pixelPos =
        engine.ConvertWorldPositionToPixels(Position);

    int scale = GetScale();

    engine.DrawPartialSprite(
        pixelPos.x - (frameWidth * scale) / 2,
        pixelPos.y - (frameHeight * scale) / 2,
        sheet,
        x,
        y,
        frameWidth,
        frameHeight,
        scale
    );
}

void PowerUp::Apply(Player& player)
{
    temp = 0.0f;
    switch (type)
    {
    case PowerUpType::Heal:
        player.SetHealth(
            std::min(player.GetHealth() + 10.0f, 100.0f)
        );
        temp = 10.0f;
        break;

    case PowerUpType::Speed:
        player.ActivateSpeedPowerup();
        temp = player.GetSpeedMultiplier();
        break;

    case PowerUpType::Damage:
        player.ActivateDamagePowerup();
        temp = player.GetDamageMultiplier();
        break;

    case PowerUpType::Shield:
        player.AddShield();
        temp = player.GetShield();
        break;
    }
}

void PowerUp::Update(AcademiaEngine& engine, float elapsedTime) {
    if (collected)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << temp;

        auto text = std::string(GetTypeName(GetType())) + " : " + oss.str();

        gameManager->AddPowerUpNotification(text);
    }

    _animationTimer += elapsedTime;

    if (_animationTimer >= 0.1f)
    {
        _animationTimer = 0.0f;

        Frame++;
        Frame %= 16;   // 4x4 = 16 frames
    }

    //Collider
    if (collider)
        collider->position = Position;
}