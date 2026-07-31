#include "AcademiaEngine.h"
#include "../Game/GameManager.h"

static AcademiaEngine* _Instance = nullptr;

AcademiaEngine::AcademiaEngine()
{
    sAppName = "AWAKENING OF THE VOID";
}

bool AcademiaEngine::OnUserCreate(){
    srand(static_cast<unsigned int>(time(nullptr)));
    SetPixelMode(olc::Pixel::ALPHA);
    RedSlimeSheet.SetSampleMode(olc::Sprite::Mode::NORMAL);
    YellowSlimeSheet.SetSampleMode(olc::Sprite::Mode::NORMAL);
    BulletSheet.SetSampleMode(olc::Sprite::Mode::NORMAL);
    PlayerSheet.SetSampleMode(olc::Sprite::Mode::NORMAL);
    PowerUpSheet.SetSampleMode(olc::Sprite::Mode::NORMAL);
    EyeChtulhuSheet.SetSampleMode(olc::Sprite::Mode::NORMAL);
    SplashBG.SetSampleMode(olc::Sprite::Mode::NORMAL);
    SplashShip.SetSampleMode(olc::Sprite::Mode::NORMAL);
    SplashFlames.SetSampleMode(olc::Sprite::Mode::NORMAL);
	LobbyBG.SetSampleMode(olc::Sprite::Mode::NORMAL);
	OptionsBG.SetSampleMode(olc::Sprite::Mode::NORMAL);
    EnemyBullet.SetSampleMode(olc::Sprite::Mode::NORMAL);
    RedSlimeSheet.LoadFromFile("assets/Sprite/Enemy/SlimePack/RedSlimeNoBG.png");
    YellowSlimeSheet.LoadFromFile("assets/Sprite/Enemy/SlimePack/YellowSlime.png");
    BulletSheet.LoadFromFile("assets/Sprite/SpaceShooterShipConstructor/PNG/Bullets/02.png");
    PlayerSheet.LoadFromFile("assets/Sprite/SpaceShooterShipConstructor/PNG/Example/05.png");
    PowerUpSheet.LoadFromFile("assets/Sprite/Effect/ConfusionEffect/Spritesheets/ConfusionEffect_Sheet_64x64.png");
    EyeChtulhuSheet.LoadFromFile("assets/Sprite/Enemy/Boss/EyeCthulhu/Boss.png");
    SplashBG.LoadFromFile("assets/Sprite/BackgroundSplashScreen/Void.png");
    SplashShip.LoadFromFile("assets/Sprite/Canva/Ship.png");
	SplashFlames.LoadFromFile("assets/Sprite/Canva/Flames.png");
	LobbyBG.LoadFromFile("assets/Sprite/BackGround/LobbyBG.png");
	OptionsBG.LoadFromFile("assets/Sprite/BackGround/OptionsBG.png");
    EnemyBullet.LoadFromFile("assets/Sprite/Enemy/Boss/EyeCthulhu/Projectiles/FinalProjectil.png");

    EyeCthulhuDecal =
        new olc::Decal(&EyeChtulhuSheet);
    SplashBGDecal =
        new olc::Decal(&SplashBG);
    SplashShipDecal =
        new olc::Decal(&SplashShip);
    SplashFlamesDecal =
        new olc::Decal(&SplashFlames);
	LobbyBGDecal =
		new olc::Decal(&LobbyBG);
	OptionsBGDecal =
		new olc::Decal(&OptionsBG);
    EnemyBulletDecal =
        new olc::Decal(&EnemyBullet);

    _GameManager = std::make_shared<GameManager>(this);
    _GameManager->Initialize(this);

    return true;
}

bool AcademiaEngine::OnUserUpdate(float elapsedTime)
{
    PreUpdate(elapsedTime);
    Update(elapsedTime);
    PostUpdate(elapsedTime);

    return true;
}

bool AcademiaEngine::OnUserDestroy()
{
    if (_GameManager)
        _GameManager->Uninitialize();
    
    return true;
}

void AcademiaEngine::PreUpdate(float elapsedTime)
{
    Clear(olc::BLACK);
}

void AcademiaEngine::Update(float elapsedTime)
{
	if (GetKey(olc::Key::ESCAPE).bPressed) exit(0);
    if (_State == EEngineState::SplashScreen) {
        UpdateSplashScreen(elapsedTime);
        return;
    }
    if (_State == EEngineState::Lobby) {
		UpdateLobby(elapsedTime);
        return;
    }
    if (_State == EEngineState::Options) {
        UpdateOptions(elapsedTime);
        return;
    }
	if (_State == EEngineState::Game) {
        if (_GameManager)
            _GameManager->Update(elapsedTime);
        return;
	}
}

/*SPLASHSCREEN*/
void AcademiaEngine::UpdateSplashScreen(float elapsedTime)
{
    //constant variables
    constexpr int FlameFrameCount = 24;

    //Timer updates
    _SplashTimer += elapsedTime;
    _TitleShineTimer += elapsedTime;
    _FlameTimer += elapsedTime;

	// Update flame animation frame
    if (_FlameTimer >= 0.07f)
    {
        _FlameTimer = 0.0f;

        _CurrentFlameFrame++;

        if (_CurrentFlameFrame >= FlameFrameCount)
            _CurrentFlameFrame = 0;
    }

	//Draw splash screen elements
    DrawSplashBackground();
    DrawSplashShip();
    DrawSplashTitle(true);

	//Looking for user input to transition to the game state
    if (GetKey(olc::Key::SPACE).bPressed ||
        GetMouse(0).bPressed)
    {
        _State = EEngineState::Lobby;
		InitializeLobby();
    }
}

void AcademiaEngine::DrawSplashBackground()
{
    DrawDecal(
        { 0.0f, 0.0f },
        SplashBGDecal,
        {
            float(ScreenWidth()) / SplashBG.width,
            float(ScreenHeight()) / SplashBG.height
        }
    );
}

void AcademiaEngine::DrawSplashFlames(
    const olc::vf2d& flameAnchor,
    float angle)
{
    constexpr int FlameFrameCount = 24;

    int flameWidth = SplashFlames.width / FlameFrameCount;
    int flameHeight = SplashFlames.height;

    int frame = _CurrentFlameFrame % FlameFrameCount;

    olc::vf2d sourcePos =
    {
        float(frame * flameWidth),
        0.0f
    };

    olc::vf2d sourceSize =
    {
        float(flameWidth),
        float(flameHeight)
    };

    constexpr float flameScale = 1.0f;

    olc::vf2d origin =
    {
        0.0f,
        flameHeight * 0.5f
    };

    DrawPartialRotatedDecal(
        flameAnchor,
        SplashFlamesDecal,
        angle,
        origin,
        sourcePos,
        sourceSize,
        {
            flameScale,
            flameScale
        }
    );
}

void AcademiaEngine::DrawSplashShip()
{
    float shipFloat =
        sinf(_SplashTimer * 1.5f) * 10.0f;

    constexpr float shipScale = 0.5f;

    float shipWidth =
        SplashShip.width * shipScale;

    float shipHeight =
        SplashShip.height * shipScale;

    olc::vf2d shipPos =
    {
        ScreenWidth() * 0.5f - shipWidth * 0.5f,
        ScreenHeight() * 0.60f - shipHeight * 0.5f
    };

    shipPos.y += shipFloat;

    // Draw flames first
    DrawSplashFlames(
        {
            shipPos.x + shipWidth * 0.53f,
            shipPos.y + shipHeight * 0.37f
        },
        -0.45f
    );

    DrawSplashFlames(
        {
            shipPos.x + shipWidth * 0.62f,
            shipPos.y + shipHeight * 0.42f
        },
        -0.40f
    );
    // Draw ship on top
    DrawDecal(
        shipPos,
        SplashShipDecal,
        { shipScale, shipScale }
    );
}

void AcademiaEngine::DrawSplashTitle(bool withSubTitle)
{
    std::string title = "AWAKENING OF THE VOID";
	std::string title2 = "PRESS SPACE OR MOUSE1 TO START";

    float scale = 6.0f;
	float scale2 = 3.0f;

    olc::vf2d textSize =
    {
        float(GetTextSize(title).x) * scale,
        float(GetTextSize(title).y) * scale
    };
    olc::vf2d text2Size =
    {
        float(GetTextSize(title2).x) * scale2,
        float(GetTextSize(title2).y) * scale2
    };

    olc::vf2d titlePos =
    {
        ScreenWidth() * 0.5f - textSize.x * 0.5f,
        80.0f
    };
    olc::vf2d title2Pos =
    {
        ScreenWidth() * 0.5f - text2Size.x * 0.5f,
        1000.0f
    };

    // ----------------------------
    // 1. Outer glow / shadow layers
    // ----------------------------
    DrawStringDecal(
        titlePos + olc::vf2d(5.0f, 5.0f),
        title,
        olc::Pixel(35, 0, 70, 180),
        { scale, scale }
    );

    DrawStringDecal(
        titlePos + olc::vf2d(-3.0f, 0.0f),
        title,
        olc::Pixel(70, 0, 130, 130),
        { scale, scale }
    );

    DrawStringDecal(
        titlePos + olc::vf2d(3.0f, 0.0f),
        title,
        olc::Pixel(70, 0, 130, 130),
        { scale, scale }
    );

    if (withSubTitle) {

        /*SUBTITLE*/
        DrawStringDecal(
            title2Pos + olc::vf2d(5.0f, 5.0f),
            title2,
            olc::Pixel(35, 0, 70, 180),
            { scale2, scale2 }
        );

        DrawStringDecal(
            title2Pos + olc::vf2d(-3.0f, 0.0f),
            title2,
            olc::Pixel(70, 0, 130, 130),
            { scale2, scale2 }
        );

        DrawStringDecal(
            title2Pos + olc::vf2d(3.0f, 0.0f),
            title2,
            olc::Pixel(70, 0, 130, 130),
            { scale2, scale2 }
        );
    }

    // ----------------------------
    // 2. Main title
    // ----------------------------
    DrawStringDecal(
        titlePos,
        title,
        olc::Pixel(190, 120, 255),
        { scale, scale }
    );

    if (withSubTitle) {
        DrawStringDecal(
            title2Pos,
            title2,
            olc::Pixel(190, 120, 255),
            { scale2, scale2 }
        );
    }
    // ----------------------------
    // 3. Moving shine effect
    // ----------------------------
    float shineSpeed = 380.0f;

    float shineX =
        fmodf(_TitleShineTimer * shineSpeed, textSize.x + 200.0f) - 100.0f;

    float shineCenter =
        titlePos.x + shineX;

    // Draw a few bright offset copies where the shine passes.
    for (int i = 0; i < title.length(); i++)
    {
        float charWidth = 8.0f * scale;
        float charX = titlePos.x + i * charWidth;

        float dist =
            fabsf(charX - shineCenter);

        if (dist < 80.0f)
        {
            float strength =
                1.0f - dist / 80.0f;

            uint8_t alpha =
                static_cast<uint8_t>(strength * 220.0f);

            olc::Pixel shineColor =
                olc::Pixel(245, 220, 255, alpha);

            std::string letter(1, title[i]);

            DrawStringDecal(
                {
                    charX,
                    titlePos.y
                },
                letter,
                shineColor,
                { scale, scale }
            );
        }
    }
}

/*LOBBY*/
void AcademiaEngine::InitializeLobby()
{
    _State = EEngineState::Lobby;
    _Buttons.clear();

    float buttonWidth = 240.0f;
    float buttonHeight = 56.0f;
    float spacing = 72.0f;

    float centerX = ScreenWidth() * 0.5f;
    float startY = ScreenHeight() * 0.52f;

    auto startButton = std::make_unique<UIButton>(this);
    startButton->SetPosition({ centerX - buttonWidth * 0.5f, startY });
    startButton->SetSize({ buttonWidth, buttonHeight });
    startButton->SetText("START");
    startButton->SetTextScale({ 2.0f, 2.0f });

    auto optionsButton = std::make_unique<UIButton>(this);
    optionsButton->SetPosition({ centerX - buttonWidth * 0.5f, startY + spacing });
    optionsButton->SetSize({ buttonWidth, buttonHeight });
    optionsButton->SetText("OPTIONS");
    optionsButton->SetTextScale({ 2.0f, 2.0f });

    auto exitButton = std::make_unique<UIButton>(this);
    exitButton->SetPosition({ centerX - buttonWidth * 0.5f, startY + spacing * 2.0f });
    exitButton->SetSize({ buttonWidth, buttonHeight });
    exitButton->SetText("EXIT");
    exitButton->SetTextScale({ 2.0f, 2.0f });

    startButton->SetOnClick([this] {
        _State = EEngineState::Game;
        });

    optionsButton->SetOnClick([this] {
        _PendingState = EEngineState::Options;
        });

    exitButton->SetOnClick([this] {
        exit(0);
        });

    _Buttons.push_back(std::move(startButton));
    _Buttons.push_back(std::move(optionsButton));
    _Buttons.push_back(std::move(exitButton));
}

void AcademiaEngine::DrawLobbyGrid()
{
    olc::Pixel gridColor = olc::Pixel(130, 40, 210, 45);

    int gridSize = 32;
    float offset = std::fmod(_LobbyTime * 20.0f, float(gridSize));

    for (int x = -gridSize; x < ScreenWidth() + gridSize; x += gridSize)
    {
        DrawLineDecal(
            { float(x), 0.0f },
            { float(x), float(ScreenHeight()) },
            gridColor
        );
    }

    for (int y = -gridSize; y < ScreenHeight() + gridSize; y += gridSize)
    {
        DrawLineDecal(
            { 0.0f, float(y) + offset },
            { float(ScreenWidth()), float(y) + offset },
            gridColor
        );
    }
}

void AcademiaEngine::DrawLobbyPanel()
{
    float panelWidth = 340.0f;
    float panelHeight = 290.0f;

    olc::vf2d panelPos =
    {
        ScreenWidth() * 0.5f - panelWidth * 0.5f,
        ScreenHeight() * 0.48f
    };

    olc::vf2d panelSize =
    {
        panelWidth,
        panelHeight
    };

    // Shadow
    FillRectDecal(
        panelPos + olc::vf2d(8.0f, 10.0f),
        panelSize,
        olc::Pixel(0, 0, 0, 110)
    );

    // Main panel
    FillRectDecal(
        panelPos,
        panelSize,
        olc::Pixel(12, 6, 28, 190)
    );

    // Outer glow rectangles
    DrawRectDecal(
        panelPos - olc::vf2d(3.0f, 3.0f),
        panelSize + olc::vf2d(6.0f, 6.0f),
        olc::Pixel(150, 45, 255, 70)
    );

    DrawRectDecal(
        panelPos,
        panelSize,
        olc::Pixel(235, 220, 255, 130)
    );

    // Top highlight
    FillRectDecal(
        panelPos + olc::vf2d(3.0f, 3.0f),
        { panelSize.x - 6.0f, 2.0f },
        olc::Pixel(255, 210, 255, 120)
    );

    // Bottom dark edge
    FillRectDecal(
        { panelPos.x + 3.0f, panelPos.y + panelSize.y - 5.0f },
        { panelSize.x - 6.0f, 2.0f },
        olc::Pixel(0, 0, 0, 130)
    );
}

void AcademiaEngine::DrawFooter()
{
    std::string footer = "v0.2  |  AWAKENING OF THE VOID";
    olc::vf2d scale = { 1.0f, 1.0f };

    olc::vi2d textSize = GetTextSize(footer);

    DrawStringDecal(
        {
            ScreenWidth() - textSize.x * scale.x - 12.0f,
            ScreenHeight() - 18.0f
        },
        footer,
        olc::Pixel(155, 120, 190),
        scale
    );
}

void AcademiaEngine::DrawLobby()
{
    //BACKGROUND
    DrawDecal(
        { 0.0f, 0.0f },
        LobbyBGDecal,
        {
            float(ScreenWidth()) / LobbyBG.width,
            float(ScreenHeight()) / LobbyBG.height
        }
    );

    DrawLobbyGrid();
    DrawLobbyPanel();

	//BUTTONS
    for (auto& buttonPtr : _Buttons)
    {
        auto& button = *buttonPtr;

        button.Draw();
    }

    DrawSplashTitle(false);
    DrawFooter();
}

void AcademiaEngine::UpdateLobby(float elapsedTime)
{
    _LobbyTime += elapsedTime;
    _TitleShineTimer += elapsedTime;

    for (auto& buttonPtr : _Buttons)
    {
        auto& button = *buttonPtr;

        button.Update(elapsedTime);

        if (button.OnHover())
        {
            if (GetMouse(0).bPressed)
                button.ExecuteOnClick();
        }
    }

    if (_PendingState == EEngineState::Options)
    {
        InitializeOptions();
    }

    DrawLobby();
}

/*OPTIONS*/
void AcademiaEngine::InitializeOptions()
{
    InitializeOptionsParticles();
    _State = EEngineState::Options;
	_Buttons.clear();
	//LOBBY BUTTON TO GO BACK TO LOBBY STATE
    auto lobbyButton = std::make_unique<UIButton>(this);
    lobbyButton->SetPosition(olc::vf2d(ScreenWidth() / 2 - 100, ScreenHeight() / 2));
    lobbyButton->SetSize(olc::vf2d(200, 50));
    lobbyButton->SetText("BACK");
	lobbyButton->SetTextScale(olc::vf2d(2.0f, 2.0f));

    auto difficultyButton = std::make_unique<UIButton>(this);
    difficultyButton->SetPosition(olc::vf2d(ScreenWidth() / 2 - 100, ScreenHeight() / 2 + 100));
    difficultyButton->SetSize(olc::vf2d(200, 75));
    difficultyButton->SetText("DIFFICULTY");
	difficultyButton->SetTextProvider([this] {
		return "DIFFICULTY:\n   " + _GameManager->GetDifficultyLevelString();
		});
    difficultyButton->SetTextScale(olc::vf2d(1.9f, 1.9f));

    lobbyButton->SetOnClick([this] {
        _PendingState = EEngineState::Lobby;
        });
    difficultyButton->SetOnClick([this]
        {
            _GameManager->SetDifficultyLevel(
                _GameManager->NextDifficulty()
            );
        });

    _Buttons.push_back(std::move(lobbyButton));
	_Buttons.push_back(std::move(difficultyButton));
}

void AcademiaEngine::InitializeOptionsParticles()
{
    _OptionsParticles.clear();

    int particleCount = 120;

    for (int i = 0; i < particleCount; i++)
    {
        OptionsParticle p;

        p.Position =
        {
            float(rand() % ScreenWidth()),
            float(rand() % ScreenHeight())
        };

        p.Velocity =
        {
            -8.0f + float(rand() % 1600) / 100.0f,  // -8 to +8
             8.0f + float(rand() % 1200) / 100.0f   //  8 to +20
        };

        p.Size = 1.0f + float(rand() % 3);          // 1 to 3 pixels
        p.Alpha = 40.0f + float(rand() % 90);       // 40 to 130
        p.PulseOffset = float(rand() % 628) / 100.0f;

        _OptionsParticles.push_back(p);
    }
}

void AcademiaEngine::DrawOptionsParticles()
{
    for (auto& p : _OptionsParticles)
    {
        float pulse = 0.5f + 0.5f * std::sin(_OptionsTime * 2.5f + p.PulseOffset);

        uint8_t alpha = uint8_t(p.Alpha + pulse * 80.0f);

        olc::Pixel color =
        {
            190,
            90,
            255,
            alpha
        };

        FillRectDecal(
            p.Position,
            { p.Size, p.Size },
            color
        );

        // Every few particles get a tiny glow cross
        if (int(p.PulseOffset * 100.0f) % 5 == 0)
        {
            DrawLineDecal(
                { p.Position.x - 2.0f, p.Position.y },
                { p.Position.x + 2.0f, p.Position.y },
                olc::Pixel(220, 160, 255, alpha / 2)
            );

            DrawLineDecal(
                { p.Position.x, p.Position.y - 2.0f },
                { p.Position.x, p.Position.y + 2.0f },
                olc::Pixel(220, 160, 255, alpha / 2)
            );
        }
    }
}

void AcademiaEngine::DrawOptions()
{
	//BACKGROUND
	DrawDecal(
		{ 0.0f, 0.0f },
		OptionsBGDecal,
		{
			float(ScreenWidth()) / OptionsBG.width,
			float(ScreenHeight()) / OptionsBG.height
		}
	);

    DrawOptionsParticles();

    //BUTTONS
    for (auto& buttonPtr : _Buttons)
    {
        auto& button = *buttonPtr;

        button.Draw();
    }

    DrawFooter();
}

void AcademiaEngine::UpdateOptions(float elapsedTime)
{
    UpdateOptionsParticles(elapsedTime);

    for (auto& buttonPtr : _Buttons)
    {
        auto& button = *buttonPtr;

        button.Update(elapsedTime);

        if (button.OnHover())
        {
            if (GetMouse(0).bPressed)
                button.ExecuteOnClick();
        }
    }

    if (_PendingState == EEngineState::Lobby)
    {
        InitializeLobby();
    }

	DrawOptions();
}

void AcademiaEngine::UpdateOptionsParticles(float dt)
{
    for (auto& p : _OptionsParticles)
    {
        p.Position += p.Velocity * dt;

        // Wrap around screen
        if (p.Position.x < 0.0f)
            p.Position.x = float(ScreenWidth());

        if (p.Position.x > ScreenWidth())
            p.Position.x = 0.0f;

        if (p.Position.y < 0.0f)
            p.Position.y = float(ScreenHeight());

        if (p.Position.y > ScreenHeight())
            p.Position.y = 0.0f;
    }
}

/*Academia*/
void AcademiaEngine::PostUpdate(float elapsedTime)
{
}

olc::vi2d AcademiaEngine::ConvertWorldPositionToPixels(const olc::vf2d& worldPosition) const
{
    int pixelx = static_cast<int>(worldPosition.x) + GetScreenSize().x / 2;
    int pixely = -static_cast<int>(worldPosition.y) + GetScreenSize().y / 2;
    return {pixelx, pixely};
}

olc::vf2d AcademiaEngine::ConvertPixelsToWorldPosition(const olc::vi2d& pixelsPosition) const
{
    float positionx = static_cast<float>(pixelsPosition.x) - static_cast<float>(GetScreenSize().x) * 0.5f;
    float positiony =  static_cast<float>(GetScreenSize().y) * 0.5f - static_cast<float>(pixelsPosition.y);
    return {positionx, positiony};
}

AcademiaEngine* AcademiaEngine::Instantiate()
{
    if (!_Instance)
    {
        _Instance = new AcademiaEngine;
    }

    return _Instance;
}

AcademiaEngine* AcademiaEngine::GetInstance()
{
    return _Instance;
}

void AcademiaEngine::UpdateDialogue(
    float elapsedTime,
    const std::vector<std::string>& dialogue)
{
    if (!_ShowDialogue)
        return;

    if (_DialogueIndex >= dialogue.size())
    {
        _ShowDialogue = false;
        return;
    }

    _CharacterTimer += elapsedTime;

    const std::string& currentLine = dialogue[_DialogueIndex];

    if (_CharacterTimer >= 0.035f)
    {
        _CharacterTimer = 0.0f;

        if (_VisibleCharacters < currentLine.size())
        {
            _VisibleCharacters++;
            _VisibleDialogueText = currentLine.substr(0, _VisibleCharacters);
        }
    }

    bool pressedContinue =
        GetKey(olc::Key::SPACE).bPressed ||
        GetKey(olc::Key::ENTER).bPressed;

    if (pressedContinue)
    {
        // If text is still typing, instantly reveal it
        if (_VisibleCharacters < currentLine.size())
        {
            _VisibleCharacters = currentLine.size();
            _VisibleDialogueText = currentLine;
        }
        else
        {
            // Otherwise go to next dialogue line
            _DialogueIndex++;
            _VisibleCharacters = 0;
            _CharacterTimer = 0.0f;

            if (_DialogueIndex >= dialogue.size())
            {
                _ShowDialogue = false;
            }
        }
    }
}

void AcademiaEngine::DrawTutorial(
    const std::string& name,
    olc::vf2d scale,
    const std::vector<std::string>& dialogue)
{
    if (!_ShowDialogue)
        return;

    int boxX = 200;
    int boxY = ScreenHeight() - 200;
    int boxW = ScreenWidth() - 250;
    int boxH = 150;

    FillRect(
        boxX,
        boxY,
        boxW,
        boxH,
        olc::VERY_DARK_BLUE
    );

    DrawRect(
        boxX,
        boxY,
        boxW,
        boxH,
        olc::WHITE
    );

    // Speaker name
    DrawString(
        boxX + 30,
        boxY + 20,
        name,
        olc::RED,
        scale.x
    );

    // Dialogue text
    DrawString(
        boxX + 30,
        boxY + 60,
        _VisibleDialogueText,
        olc::WHITE,
        scale.x
    );

    // Skip / continue prompt
    if (_DialogueIndex < dialogue.size())
    {
        bool lineComplete = _VisibleCharacters >= dialogue[_DialogueIndex].size();

        if (lineComplete)
        {
            DrawString(
                boxX + 30,
                boxY + 120,
                "Press SPACE or ENTER to continue",
                olc::YELLOW,
                1
            );
        }
        else
        {
            DrawString(
                boxX + 30,
                boxY + 120,
                "Press SPACE or ENTER to skip",
                olc::GREY,
                1
            );
        }
    }
}

void AcademiaEngine::DrawProfilBox() {
    // -------------------------------------------------
    // Shadow / depth
    // -------------------------------------------------
    FillRectDecal(
        pos + olc::vf2d(4.0f, 5.0f),
        size,
        shadow
    );

    // -------------------------------------------------
    // Main background base
    // -------------------------------------------------
    FillRectDecal(pos, size, deepPurple);

    // -------------------------------------------------
    // Fake vertical gradient
    // -------------------------------------------------
    for (int y = 0; y < int(size.y); y++)
    {
        float k = float(y) / size.y;

        int r = int(70 + (120 * (1.0f - k)));
        int g = int(20 + (25 * (1.0f - k)));
        int b = int(120 + (90 * (1.0f - k)));

        FillRectDecal(
            { pos.x, pos.y + float(y) },
            { size.x, 1.0f },
            olc::Pixel(r, g, b)
        );
    }

    // -------------------------------------------------
    // Top highlight strip
    // -------------------------------------------------
    FillRectDecal(
        pos + olc::vf2d(2.0f, 2.0f),
        { size.x - 4.0f, 2.0f },
        olc::Pixel(255, 210, 255, 110)
    );

    // -------------------------------------------------
    // Bottom depth strip
    // -------------------------------------------------
    FillRectDecal(
        { pos.x + 2.0f, pos.y + size.y - 4.0f },
        { size.x - 4.0f, 2.0f },
        olc::Pixel(20, 5, 40, 180)
    );

    // -------------------------------------------------
    // Border
    // -------------------------------------------------
    DrawRectDecal(
        pos,
        size,
        glowPurple
    );

    // Inner dark border, gives a bevel feel
    DrawRectDecal(
        pos + olc::vf2d(1.0f, 1.0f),
        size - olc::vf2d(2.0f, 2.0f),
        olc::Pixel(30, 8, 55, 140)
    );
}

bool AcademiaEngine::IsDialogueLineComplete(const std::vector<std::string>& dialogue) const
{
    if (!_ShowDialogue)
        return false;

    if (_DialogueIndex >= dialogue.size())
        return false;

    return _VisibleCharacters >= dialogue[_DialogueIndex].size();
}