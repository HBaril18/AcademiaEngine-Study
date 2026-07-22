#include "AcademiaEngine.h"
#include "../Game/GameManager.h"

static AcademiaEngine* _Instance = nullptr;

AcademiaEngine::AcademiaEngine()
{
    sAppName = "AcademiaEngine";
}

bool AcademiaEngine::OnUserCreate(){
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
    DrawSplashTitle();

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

void AcademiaEngine::DrawSplashTitle()
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

    // ----------------------------
    // 2. Main title
    // ----------------------------
    DrawStringDecal(
        titlePos,
        title,
        olc::Pixel(190, 120, 255),
        { scale, scale }
    );

    DrawStringDecal(
        title2Pos,
        title2,
        olc::Pixel(190, 120, 255),
        { scale2, scale2 }
    );

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
    //OPTIONS BUTTON TO GO IN OPTIONS STATE
    auto optionsButton = std::make_unique<UIButton>(this);
    optionsButton->SetPosition(olc::vf2d(ScreenWidth() / 2 - 100, ScreenHeight() / 2));
    optionsButton->SetSize(olc::vf2d(200, 50));
    optionsButton->SetText("OPTIONS");
    optionsButton->SetTextScale(olc::vf2d(2.0f, 2.0f));

	//START BUTTON TO GO IN GAME STATE
    auto startButton = std::make_unique<UIButton>(this);
    startButton->SetPosition(olc::vf2d(ScreenWidth() / 2 - 100, ScreenHeight() / 2 + 100));
    startButton->SetSize(olc::vf2d(200, 50));
    startButton->SetText("START");
    startButton->SetTextScale(olc::vf2d(2.0f, 2.0f));

	//EXIT BUTTON TO EXIT THE GAME
    auto exitButton = std::make_unique<UIButton>(this);
    exitButton->SetPosition(olc::vf2d(ScreenWidth() / 2 - 100, ScreenHeight() / 2 + 400));
    exitButton->SetSize(olc::vf2d(200, 50));
    exitButton->SetText("EXIT");
    exitButton->SetTextScale(olc::vf2d(2.0f, 2.0f));

    optionsButton->SetOnClick([this] {
        _PendingState = EEngineState::Options;
        });
    startButton->SetOnClick([this] {
        _State = EEngineState::Game;
        });
	exitButton->SetOnClick([this] {
		exit(0);
		});

	_Buttons.push_back(std::move(optionsButton));
    _Buttons.push_back(std::move(startButton));
	_Buttons.push_back(std::move(exitButton));
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

	//BUTTONS
    for (auto& buttonPtr : _Buttons)
    {
        auto& button = *buttonPtr;

        button.Draw();
    }
}

void AcademiaEngine::UpdateLobby(float elapsedTime)
{
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

    //BUTTONS
    for (auto& buttonPtr : _Buttons)
    {
        auto& button = *buttonPtr;

        button.Draw();
    }

}

void AcademiaEngine::UpdateOptions(float elapsedTime)
{
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