#include "Cthulhu.h"

Cthulhu::Cthulhu(olc::vf2d pos)
    : Ennemies(pos, 50.0f, 500.0f)
{
    _MovementState = CthulhuMovementState::Intro;

    Scale = 4;
    MaxHealth = 500.0f;
    Health = MaxHealth;
    _MovementStateTimer = 0.0f;
    _DialogueIndex = 0;
    _ShowDialogue = false;

    collider = std::make_unique<Collider>();

    collider->owner = this;
    collider->position = Position;
    collider->size = Radius;
    collider->layer = 2;
    collider->type = Collider::EColliderType::Circle;
    collider->enabled = false;

    if (collisionManager) {
        collisionManager->RegisterCollider(collider.get());
    }
}

void Cthulhu::Draw(AcademiaEngine& engine)
{
    constexpr int frameWidth = 32;
    constexpr int frameHeight = 24;

    olc::vi2d pixelPos =
        engine.ConvertWorldPositionToPixels(Position);

    olc::vf2d drawPos =
    {
        (float)pixelPos.x,
        (float)pixelPos.y
    };

    int column = _CurrentFrame % 3;
    int row = _CurrentFrame / 3;

    olc::vi2d sourcePos =
    {
        column * frameWidth,
        row * frameHeight
    };

    engine.DrawPartialRotatedDecal(
        drawPos,
        engine.EyeCthulhuDecal,
        _LookAngle,
        {
            frameWidth * 0.5f,
            frameHeight * 0.5f
        },
        sourcePos,
        {
            (float)frameWidth,
            (float)frameHeight
        },
        {
            (float)Scale,
            (float)Scale
        }
    );
}

void Cthulhu::Update(AcademiaEngine& engine, float elapsedTime)
{
    if (_MovementState != CthulhuMovementState::Intro &&
        !_ShowDialogue)
    {
        _MovementStateTimer += elapsedTime;

        if (_MovementStateTimer >= 10.0f)
        {
            _MovementStateTimer = 0.0f;

            int next = 1 + rand() % 5;

            _MovementState =
                static_cast<CthulhuMovementState>(next);
        }
    }

    switch (_MovementState)
    {
    case CthulhuMovementState::Intro:
        collider->enabled = false;
        UpdateIntro(elapsedTime, engine);
        break;

    case CthulhuMovementState::Orbit:
        collider->enabled = true;
        UpdateOrbit(elapsedTime);
        break;

    case CthulhuMovementState::Sweep:
        collider->enabled = true;
        UpdateSweep(elapsedTime, engine);
        break;

    case CthulhuMovementState::Figure8:
        collider->enabled = true;
        UpdateFigure8(elapsedTime);
        break;

    case CthulhuMovementState::Teleport:
        collider->enabled = true;
        UpdateTeleport(elapsedTime);
        break;

    case CthulhuMovementState::Summon:
        collider->enabled = false;
        UpdateSummons(elapsedTime);
        break;
    }

    // Animation
    _AnimationTimer += elapsedTime;

    if (_AnimationTimer >= _AnimationSpeed)
    {
        _AnimationTimer = 0.0f;

        _CurrentFrame++;

        if (_CurrentFrame >= 7)
            _CurrentFrame = 0;
    }

    constexpr float PI = 3.14159265f;

    // Figure8 = look along movement
    if (_MovementState == CthulhuMovementState::Figure8)
    {
        olc::vf2d velocity =
            Position - _PreviousPosition;

        if (velocity.mag() > 0.1f)
        {
            velocity = velocity.norm();

            _LookAngle =
                atan2f(
                    -velocity.y,
                    velocity.x
                ) + PI;
        }
    }
    // All other states = look at player
    else
    {
        olc::vf2d toPlayer =
            GetPlayer()->GetPosition() - Position;

        if (toPlayer.mag() > 0.1f)
        {
            toPlayer = toPlayer.norm();

            _LookAngle =
                atan2f(-toPlayer.y, toPlayer.x) + PI;
        }
    }

    _PreviousPosition = Position;

    if (collider)
        collider->position = Position;
}

void Cthulhu::UpdateIntro(float elapsedTime, AcademiaEngine & engine)
{
    if ((Position - _TargetIntroPosition).mag() < 5.0f)
    {
        _ShowDialogue = true;
    }

    if (!_IntroInitialized)
    {
        olc::vf2d topCenterWorld =
            engine.ConvertPixelsToWorldPosition(
                {
                    engine.ScreenWidth() / 2,
                    -50
                }
            );

        Player* player = GetPlayer();

        if (!player)
            return;

        auto playerPos =
            GetPlayer()->GetPosition();

        Position = topCenterWorld;

        _TargetIntroPosition =
            engine.ConvertPixelsToWorldPosition(
                {
                    engine.ScreenWidth() / 2,
                    150
                }
            );

        _IntroInitialized = true;
    }

    olc::vf2d dir =
        _TargetIntroPosition - Position;

    float dist = dir.mag();

    if (dist > 5.0f)
    {
        Position +=
            dir.norm() *
            300.0f *
            elapsedTime;
    }
    else
    {
        Position = _TargetIntroPosition;

        if (_ShowDialogue)
        {
            UpdateDialogue(elapsedTime);
        }
    }
}

bool Cthulhu::IsDialogueActive() const
{
    return _ShowDialogue;
}

void Cthulhu::DrawDialogue(AcademiaEngine& engine)
{
    if (!_ShowDialogue)
        return;

    // Background box
    engine.FillRect(
        50,
        engine.ScreenHeight() - 200,
        engine.ScreenWidth() - 100,
        150,
        olc::VERY_DARK_BLUE
    );

    engine.DrawRect(
        50,
        engine.ScreenHeight() - 200,
        engine.ScreenWidth() - 100,
        150,
        olc::WHITE
    );

    // Speaker
    engine.DrawString(
        80,
        engine.ScreenHeight() - 180,
        "EYE OF CTHULHU",
        olc::RED,
        2
    );

    // Current line
    if (_DialogueIndex < _Dialogue.size())
    {
        std::string visibleText =
            _Dialogue[_DialogueIndex].substr(
                0,
                _VisibleCharacters
            );

        engine.DrawString(
            80,
            engine.ScreenHeight() - 130,
            visibleText,
            olc::WHITE,
            2
        );
    }
}

void Cthulhu::UpdateDialogue(float elapsedTime) {
    DialogueTimer += elapsedTime;

    _CharacterTimer += elapsedTime;

    if (_CharacterTimer >= 0.05f)
    {
        _CharacterTimer = 0.0f;

        if (_VisibleCharacters <
            _Dialogue[_DialogueIndex].size())
        {
            _VisibleCharacters++;
        }
    }

    if (DialogueTimer >= 2.0f)
    {
        DialogueTimer = 0.0f;

        _DialogueIndex++;
        _DialogueIndex++;
        _VisibleCharacters = 0;

        if (_DialogueIndex >= _Dialogue.size())
        {
            _ShowDialogue = false;
            _MovementState = CthulhuMovementState::Orbit;
        }
    }
}

void Cthulhu::UpdateOrbit(float elapsedTime)
{
    _OrbitAngle += elapsedTime;

    Player* player = GetPlayer();

    if (!player)
        return;

    auto playerPos = player->GetPosition();

    float radius = 250.0f;

    olc::vf2d orbitTarget =
    {
        playerPos.x + cosf(_OrbitAngle) * radius,
        playerPos.y + sinf(_OrbitAngle) * radius
    };

    olc::vf2d dir = orbitTarget - Position;

    Position += dir * 2.0f * elapsedTime;
}

void Cthulhu::UpdateSweep(float elapsedTime, AcademiaEngine& engine)
{
    olc::vf2d sweepWorldPos =
        engine.ConvertPixelsToWorldPosition(
            {
                engine.ScreenWidth() / 2,
                engine.ScreenHeight() - 150
            });

    Position.y = sweepWorldPos.y;

    Position.x +=
        _SweepDirection *
        500.0f *
        elapsedTime;

    constexpr float LeftLimit = -700.0f;
    constexpr float RightLimit = 700.0f;

    if (Position.x <= LeftLimit)
    {
        Position.x = LeftLimit;
        _SweepDirection = 1.0f;
    }

    if (Position.x >= RightLimit)
    {
        Position.x = RightLimit;
        _SweepDirection = -1.0f;
    }
}

void Cthulhu::UpdateFigure8(float elapsedTime)
{
    std::cout << "Cthulhu::UpdateFigure8\n";
    _PatternTimer += elapsedTime;

    Player* player = GetPlayer();

    if (!player)
    {
        std::cout << "PLAYER NULL\n";
        return;
    }

    auto playerPos =
        GetPlayer()->GetPosition();

    Position.x =
        playerPos.x +
        sinf(_PatternTimer) * 300.0f;

    Position.y =
        playerPos.y +
        sinf(_PatternTimer * 2.0f) * 150.0f;
}

void Cthulhu::UpdateTeleport(float elapsedTime)
{
    std::cout << "Cthulhu::UpdateTeleport\n";
    _TeleportTimer += elapsedTime;

    if (_TeleportTimer >= 2.0f)
    {
        _TeleportTimer = 0.0f;

        float angle =
            (float)rand() /
            RAND_MAX *
            6.283185f;

        Player* player = GetPlayer();

        if (!player)
            return;

        auto playerPos =
            GetPlayer()->GetPosition();

        Position =
        {
            playerPos.x +
            cosf(angle) * 350.0f,

            playerPos.y +
            sinf(angle) * 350.0f
        };
    }
}

void Cthulhu::UpdateSummons(float elapsedTime)
{
    std::cout << "Cthulhu::UpdateSummons\n";
    _SummonTimer += elapsedTime;

    if (_SummonTimer >= 8.0f)
    {
        _SummonTimer = 0.0f;

        if (gameManager)
        {
            gameManager->SpawnEnemy(
                EEnemyType::Basic,
                Position +
                olc::vf2d(80.0f, 0.0f));

            gameManager->SpawnEnemy(
                EEnemyType::Fast,
                Position +
                olc::vf2d(-80.0f, 0.0f));
        }
    }
}