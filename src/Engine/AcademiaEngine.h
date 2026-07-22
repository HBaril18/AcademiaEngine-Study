#pragma once

#include "../../external/olc/olcPixelGameEngine.h"
#include "../Utils.h"
#include "../../UIButton.h"
#include <deque>
#include "../../EngineState.h"

class GameManager;
class UIButton;

struct SplashPixel
{
    olc::vf2d position;
    olc::vf2d target;
    olc::vf2d velocity;
    float delay = 0.0f;
    bool reached = false;
};

class AcademiaEngine : public olc::PixelGameEngine
{
public:
    AcademiaEngine();

    bool OnUserCreate() override;
    bool OnUserUpdate(float elapsedTime) override;

    olc::vi2d ConvertWorldPositionToPixels(const olc::vf2d& worldPosition) const;
    olc::vf2d ConvertPixelsToWorldPosition(const olc::vi2d& pixelsPosition) const;

    static AcademiaEngine* Instantiate();
    static AcademiaEngine* GetInstance();

    //SPLASHSCREEN
    void UpdateSplashScreen(float elapsedTime);
	void DrawSplashBackground();
	void DrawSplashShip();
	void DrawSplashFlames(const olc::vf2d& shipPos, float angle);
	void DrawSplashTitle();

	//LOBBY
	void InitializeLobby();
	void DrawLobby();
	void UpdateLobby(float elapsedTime);

    //OPTIONS
	void InitializeOptions();
	void DrawOptions();
	void UpdateOptions(float elapsedTime);

    //GAMESTATE
	void SetState(EEngineState state) { _State = state; }
	EEngineState GetState() const { return _State; }
	EEngineState GetPendingState() const { return _PendingState; }

    olc::Sprite RedSlimeSheet;
    olc::Sprite YellowSlimeSheet;
    olc::Sprite BulletSheet;
    olc::Sprite PlayerSheet;
    olc::Sprite PowerUpSheet;
    olc::Sprite EyeChtulhuSheet;
    olc::Sprite SplashBG;
    olc::Sprite SplashShip;
    olc::Sprite SplashFlames;
	olc::Sprite LobbyBG;
	olc::Sprite OptionsBG;

    olc::Decal* EyeCthulhuDecal = nullptr;
    olc::Decal* SplashBGDecal = nullptr;
    olc::Decal* SplashShipDecal = nullptr;
    olc::Decal* SplashFlamesDecal = nullptr;
	olc::Decal* LobbyBGDecal = nullptr;
	olc::Decal* OptionsBGDecal = nullptr;

private:

    void PreUpdate(float elapsedTime);
    void Update(float elapsedTime);
    void PostUpdate(float elapsedTime);

    EEngineState _State = EEngineState::SplashScreen;
	EEngineState _PendingState;
    float _SplashTimer = 0.0f;

    GameManagerPtr _GameManager = nullptr;

    std::vector<SplashPixel> _SplashPixels;
    bool _SplashInitialized = false;

    float _FlameTimer = 0.0f;
    int _CurrentFlameFrame = 0;

    float _TitleShineTimer = 0.0f;

    float _ButtonLineAnim = 0.0f;
    float _AnimSpeed = 2.0f;

    std::deque<std::unique_ptr<UIButton>> _Buttons;
};
