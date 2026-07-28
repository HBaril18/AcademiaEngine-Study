#include "Cthulhu.h"

Cthulhu::Cthulhu(olc::vf2d pos)
    : Ennemies(pos, 50.0f, 500.0f)
{
    _MovementState = CthulhuMovementState::Intro;

    Scale = 4;
    MaxHealth = 10.0f;
    Health = MaxHealth;
    _MovementStateTimer = 0.0f;
    _DialogueIndex = 0;
    _ShowDialogue = false;
}

Cthulhu::~Cthulhu()
{
    if (collisionManager && collider)
    {
        collisionManager->UnregisterCollider(collider);
    }

    delete collider;
    collider = nullptr;
}

void Cthulhu::InitializeCollision(CollisionManager* collisionManager)
{
    Ennemies::InitializeCollision(collisionManager);

    if (collider)
    {
        collider->enabled = false;
    }
    _CanTakeDamage = false;
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

    DrawBossHealthBar(engine, GetHealth(), MaxHealth);
}

void Cthulhu::TakeDamage(float damage, float elapsedTime)
{
	if (_CanTakeDamage)
	{
		Health -= damage;
		Health = std::max(Health, 0.0f);
	}
    if (Health <= 0.0f && State == BossState::Alive) {
        Health = 0.0f;
        Player* p = GetPlayer();
        p->AddScore(1500.0f);

        // disable collider immediately to avoid further collision processing
        if (collider) collider->enabled = false;
    }
}

void Cthulhu::DrawBossHealthBar(AcademiaEngine& engine, float health, float maxHealth)
{
    // -------------------------------------------------
    // Bar setup
    // -------------------------------------------------
    olc::vi2d barSize = { 760, 24 };

    olc::vi2d barPos =
    {
        engine.ScreenWidth() / 2 - barSize.x / 2,
        75
    };

    float healthRatio = 0.0f;

    if (maxHealth > 0.0f)
        healthRatio = health / maxHealth;

    healthRatio = std::clamp(healthRatio, 0.0f, 1.0f);

    int fillWidth = int(float(barSize.x) * healthRatio);

    bool danger = healthRatio < 0.25f;

    // -------------------------------------------------
    // Colors
    // -------------------------------------------------
    olc::Pixel darkPanel = olc::Pixel(10, 4, 22);
    olc::Pixel frameDark = olc::Pixel(45, 10, 80);
    olc::Pixel purple = olc::Pixel(170, 45, 255);
    olc::Pixel lightPurple = olc::Pixel(235, 190, 255);
    olc::Pixel red = olc::Pixel(190, 20, 45);
    olc::Pixel hotRed = olc::Pixel(255, 45, 85);
    olc::Pixel lowHealth = olc::Pixel(255, 60, 180);
    olc::Pixel black = olc::Pixel(0, 0, 0);

    olc::Pixel healthColor = healthRatio < 0.25f ? lowHealth : hotRed;

    // -------------------------------------------------
    // Shadow behind bar
    // -------------------------------------------------
    engine.FillRect(
        barPos + olc::vi2d(6, 7),
        barSize,
        olc::Pixel(0, 0, 0)
    );

    // -------------------------------------------------
    // Outer glow / frame
    // -------------------------------------------------
    engine.DrawRect(
        barPos - olc::vi2d(4, 4),
        barSize + olc::vi2d(8, 8),
        frameDark
    );

    engine.DrawRect(
        barPos - olc::vi2d(2, 2),
        barSize + olc::vi2d(4, 4),
        purple
    );

    engine.DrawRect(
        barPos - olc::vi2d(1, 1),
        barSize + olc::vi2d(2, 2),
        lightPurple
    );

	if (danger) //ADD MORE EFFECTS WHEN LOW HEALTH
    {
        engine.DrawRect(
            barPos - olc::vi2d(6, 6),
            barSize + olc::vi2d(12, 12),
            olc::Pixel(255, 40, 160)
        );

        engine.DrawRect(
            barPos - olc::vi2d(8, 8),
            barSize + olc::vi2d(16, 16),
            olc::Pixel(120, 0, 80)
        );
    }

    // -------------------------------------------------
    // Dark inner background
    // -------------------------------------------------
    engine.FillRect(
        barPos,
        barSize,
        darkPanel
    );

    // -------------------------------------------------
    // Health fill
    // -------------------------------------------------
    if (fillWidth > 0)
    {
        engine.FillRect(
            barPos,
            { fillWidth, barSize.y },
            healthColor
        );

        // Top highlight on health
        engine.FillRect(
            barPos + olc::vi2d(0, 2),
            { fillWidth, 3 },
            olc::Pixel(255, 150, 170)
        );

        // Bottom dark depth
        engine.FillRect(
            { barPos.x, barPos.y + barSize.y - 5 },
            { fillWidth, 4 },
            olc::Pixel(90, 0, 25)
        );

        // Bright leading edge
        engine.DrawLine(
            { barPos.x + fillWidth, barPos.y + 1 },
            { barPos.x + fillWidth, barPos.y + barSize.y - 2 },
            olc::Pixel(255, 220, 240)
        );
    }

    // -------------------------------------------------
    // Segment lines
    // -------------------------------------------------
    int segmentCount = 20;
    int segmentWidth = barSize.x / segmentCount;

    for (int i = 1; i < segmentCount; i++)
    {
        int x = barPos.x + i * segmentWidth;

        engine.DrawLine(
            { x, barPos.y + 3 },
            { x, barPos.y + barSize.y - 4 },
            olc::Pixel(20, 4, 35)
        );

        engine.DrawLine(
            { x + 1, barPos.y + 3 },
            { x + 1, barPos.y + barSize.y - 4 },
            olc::Pixel(120, 35, 170)
        );
    }

    // -------------------------------------------------
    // Inner border
    // -------------------------------------------------
    engine.DrawRect(
        barPos,
        barSize,
        olc::Pixel(255, 220, 255)
    );

    engine.DrawRect(
        barPos + olc::vi2d(1, 1),
        barSize - olc::vi2d(2, 2),
        olc::Pixel(60, 10, 90)
    );

    // -------------------------------------------------
    // Boss name
    // -------------------------------------------------
    std::string bossName = "Cthulhu Eye";

    olc::vi2d textSize = engine.GetTextSize(bossName);

    olc::vi2d textPos =
    {
        engine.ScreenWidth() / 2 - textSize.x,
        barPos.y - 24
    };

    // Text shadow
    engine.DrawString(
        textPos + olc::vi2d(2, 2),
        bossName,
        black,
        2
    );

    // Main text
    engine.DrawString(
        textPos,
        bossName,
        olc::Pixel(245, 220, 255),
        2
    );
}

void Cthulhu::Update(AcademiaEngine& engine, float elapsedTime)
{
    if (Health <= 0 && State == BossState::Alive)
    {
        State = BossState::Dying;
        _DeathTimer = 17.0f; // seconds
        _DialogueIndex = 0;
        _VisibleCharacters = 0;
        _ShowDialogue = true;
    }

    if (State == BossState::Dying)
    {
        _DeathTimer -= elapsedTime;

        if (_DeathTimer <= 0.0f)
            State = BossState::Dead;

        if (Health <= 0 && State == BossState::Alive)
        {
            State = BossState::Dying;
        }
    }

    if (_MovementState != CthulhuMovementState::Intro &&
        !_ShowDialogue)
    {
        _MovementStateTimer += elapsedTime;

        if (_MovementStateTimer >= 10.0f)
        {
            _MovementStateTimer = 0.0f;

            int next = 1 + rand() % 4;

            _MovementState =
                static_cast<CthulhuMovementState>(next);
        }
    }

    if (State == BossState::Dying)
    {
        UpdateDialogue(elapsedTime, GetDeathDialogue());

        if (!_ShowDialogue)
        {
            State = BossState::Dead;
        }

        return;
    }

    switch (_MovementState)
    {
    case CthulhuMovementState::Intro:
        _CanTakeDamage = false;
        SetColliderEnabled(false);
        UpdateIntro(elapsedTime, engine);
        break;

    case CthulhuMovementState::Orbit:
        _CanTakeDamage = true;
        SetColliderEnabled(true);
        UpdateOrbit(elapsedTime);
        break;

    case CthulhuMovementState::Sweep:
        _CanTakeDamage = true;
        SetColliderEnabled(true);
        UpdateSweep(elapsedTime, engine);
        break;

    case CthulhuMovementState::Teleport:
        _CanTakeDamage = true;
        SetColliderEnabled(true);
        UpdateTeleport(elapsedTime);
        break;

    case CthulhuMovementState::Summon:
        _CanTakeDamage = false;
        SetColliderEnabled(false);
        UpdateSummons(elapsedTime, engine);
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

    olc::vf2d toPlayer =
        GetPlayer()->GetPosition() - Position;

    if (toPlayer.mag() > 0.1f)
    {
        toPlayer = toPlayer.norm();
        _LookAngle =
            atan2f(-toPlayer.y, toPlayer.x) + PI;
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
            UpdateDialogue(elapsedTime, GetIntroDialogue());
        }
    }
}

bool Cthulhu::IsDialogueActive() const
{
    return _ShowDialogue;
}

void Cthulhu::DrawDialogue(AcademiaEngine& engine, std::vector<std::string> dialogue)
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
    if (_DialogueIndex < dialogue.size())
    {
        std::string visibleText =
            dialogue[_DialogueIndex].substr(
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

void Cthulhu::UpdateDialogue(float elapsedTime, std::vector<std::string> dialogue) {
    if (_DialogueIndex >= dialogue.size())
    {
        _ShowDialogue = false;
        return;
    }

    DialogueTimer += elapsedTime;

    _CharacterTimer += elapsedTime;

    if (_CharacterTimer >= 0.05f)
    {
        _CharacterTimer = 0.0f;

        if (_VisibleCharacters <
            dialogue[_DialogueIndex].size())
        {
            _VisibleCharacters++;
        }
    }

    if (DialogueTimer >= 2.0f)
    {
        DialogueTimer = 0.0f;

        _DialogueIndex++;
        _VisibleCharacters = 0;

        if (_DialogueIndex >= dialogue.size())
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

		Position = //SHOULD BE AWARE OF SCREEN BOUNDS !!!!!
        {
            playerPos.x +
            cosf(angle) * 350.0f,

            playerPos.y +
            sinf(angle) * 350.0f
        };
    }
}

void Cthulhu::UpdateSummons(float elapsedTime, AcademiaEngine& engine)
{
    _SummonTimer += elapsedTime;

    if (_SummonTimer >= 6.0f)
    {
        _SummonTimer = 0.0f;

		if (gameManager)
        {
			//Randomly spawn enemies around Cthulhu
            SummonEnemiesAround(engine);
        }
    }
}

void Cthulhu::SummonEnemiesAround(AcademiaEngine& engine)
{
    if (!gameManager)
        return;

    std::vector<olc::vf2d> plannedPositions;

    int enemyCount = 4;

    float minDistance = 120.0f;
    float maxDistance = 280.0f;
    float spawnRadius = 22.0f;

    for (int i = 0; i < enemyCount; i++)
    {
        olc::vf2d spawnPos;

        bool foundPosition =
            TryGetRandomSpawnPositionAround(
                engine,
                minDistance,
                maxDistance,
                spawnRadius,
                plannedPositions,
                spawnPos
            );

        if (!foundPosition)
        {
            continue;
        }

        plannedPositions.push_back(spawnPos);

        EEnemyType enemyType = EEnemyType::Basic;

        if (rand() % 3 == 0)
            enemyType = EEnemyType::Fast;

        gameManager->SpawnEnemy(enemyType, spawnPos);
    }
}

olc::vf2d Cthulhu::GetRandomPointAround(float minDistance, float maxDistance) {
    constexpr float PI = 3.14159265f;

    float angle = RandomFloat(0.0f, PI * 2.0f);

    // Better distribution than simple random distance
    float minD2 = minDistance * minDistance;
    float maxD2 = maxDistance * maxDistance;
    float distance = std::sqrt(RandomFloat(minD2, maxD2));

    return Position + olc::vf2d(
        std::cos(angle) * distance,
        std::sin(angle) * distance
    );
}

bool Cthulhu::IsSpawnPositionValid(
    AcademiaEngine& engine,
    const olc::vf2d& spawnPos,
    float spawnRadius,
    const std::vector<olc::vf2d>& plannedPositions
)
{
    // Keep inside screen
    float margin = 40.0f;

    olc::vi2d pixelSpawnPos = engine.ConvertWorldPositionToPixels(spawnPos);

    if (pixelSpawnPos.x < margin || pixelSpawnPos.x > engine.ScreenWidth() - margin)
        return false;

    if (pixelSpawnPos.y < margin || pixelSpawnPos.y > engine.ScreenHeight() - margin)
        return false;

    // Do not spawn too close to player
    if (GetPlayer())
    {
        if (CirclesOverlap(
            spawnPos,
            spawnRadius,
            GetPlayer()->GetPosition(),
            120.0f
        ))
        {
            return false;
        }
    }

    // Do not overlap existing enemies
    if (gameManager)
    {
        for (auto& enemy : gameManager->GetEnemies())
        {
            if (!enemy)
                continue;

            if (CirclesOverlap(
                spawnPos,
                spawnRadius,
                enemy->GetPosition(),
                enemy->GetRadius() + 20.0f
            ))
            {
                return false;
            }
        }
    }

    // Do not overlap enemies planned in this same summon wave
    for (const auto& plannedPos : plannedPositions)
    {
        if (CirclesOverlap(
            spawnPos,
            spawnRadius,
            plannedPos,
            spawnRadius + 25.0f
        ))
        {
            return false;
        }
    }

    return true;
}

bool Cthulhu::TryGetRandomSpawnPositionAround(
    AcademiaEngine& engine,
    float minDistance,
    float maxDistance,
    float spawnRadius,
    const std::vector<olc::vf2d>& plannedPositions,
    olc::vf2d& outPosition
)
{
    constexpr int maxAttempts = 40;

    for (int attempt = 0; attempt < maxAttempts; attempt++)
    {
        olc::vf2d candidate =
            GetRandomPointAround(minDistance, maxDistance);

        if (IsSpawnPositionValid(
            engine,
            candidate,
            spawnRadius,
            plannedPositions
        ))
        {
            outPosition = candidate;
            return true;
        }
    }

    return false;
}