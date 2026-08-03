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

struct OptionsParticle
{
    olc::vf2d Position;
    olc::vf2d Velocity;
    float Size = 1.0f;
    float Alpha = 80.0f;
    float PulseOffset = 0.0f;
};

enum class ETutorialStep
{
    Intro,
    Move,
    Shoot,
    Enemy,
    PowerUp,
    BossWarning,
    CompleteMessage,
    Transition,
    Finished
};

class AcademiaEngine : public olc::PixelGameEngine
{
public:
    AcademiaEngine();

    bool OnUserCreate() override;
    bool OnUserUpdate(float elapsedTime) override;
    bool OnUserDestroy() override;

    olc::vi2d ConvertWorldPositionToPixels(const olc::vf2d& worldPosition) const;
    olc::vf2d ConvertPixelsToWorldPosition(const olc::vi2d& pixelsPosition) const;

    static AcademiaEngine* Instantiate();
    static AcademiaEngine* GetInstance();

    //SPLASHSCREEN
    void UpdateSplashScreen(float elapsedTime);
	void DrawSplashBackground();
	void DrawSplashShip();
	void DrawSplashFlames(const olc::vf2d& shipPos, float angle);
	void DrawSplashTitle(bool withSubTitle);

	//LOBBY
	void InitializeLobby();
    void DrawLobbyGrid();
    void DrawLobbyPanel();
	void DrawLobby();
	void UpdateLobby(float elapsedTime);

    //OPTIONS
	void InitializeOptions();
    void InitializeOptionsParticles();
    void DrawOptionsParticles();
	void DrawOptions();
	void UpdateOptions(float elapsedTime);
    void UpdateOptionsParticles(float dt);

    void DrawFooter();

    //GAMESTATE
	void SetState(EEngineState state) { _State = state; }
	EEngineState GetState() const { return _State; }
	EEngineState GetPendingState() const { return _PendingState; }

    //DIALOGUE
    void UpdateDialogue(float elapsedTime, const std::vector<std::string>& dialogue);
    void DrawTutorial(
        const std::string& name,
        olc::vf2d scale,
        const std::vector<std::string>& dialogue);
    void DrawProfilBox();
    void StartDialogue(
        const std::string& speaker,
        const std::vector<std::string>& lines)
    {
        currentSpeakerName = speaker;
        currentTutorialDialogue = lines;

        _DialogueIndex = 0;
        _VisibleCharacters = 0;
        _CharacterTimer = 0.0f;
        DialogueTimer = 0.0f;
        _ShowDialogue = true;
    }
    bool IsDialogueLineComplete(const std::vector<std::string>& dialogue) const;
    int _DialogueIndex;
    bool _ShowDialogue;
    float _CharacterTimer = 0.0f;
    int _VisibleCharacters = 0;
    float DialogueTimer = 0.0f;
    std::string _VisibleDialogueText;
    
    olc::Pixel purple = olc::Pixel(170, 45, 255);
    olc::Pixel darkPurple = olc::Pixel(55, 15, 95);
    olc::Pixel deepPurple = olc::Pixel(22, 10, 42);
    olc::Pixel white = olc::Pixel(235, 220, 255);
    olc::Pixel glowPurple = olc::Pixel(220, 90, 255);
    olc::Pixel shadow = olc::Pixel(0, 0, 0, 120);
    olc::Pixel textShadow = olc::Pixel(20, 5, 35, 180);
    olc::vf2d pos = { 25.0f, 880.0f };
    olc::vf2d size = { 150.0f, 150.0f };

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
    olc::Sprite EnemyBullet;

    olc::Decal* EyeCthulhuDecal = nullptr;
    olc::Decal* SplashBGDecal = nullptr;
    olc::Decal* SplashShipDecal = nullptr;
    olc::Decal* SplashFlamesDecal = nullptr;
	olc::Decal* LobbyBGDecal = nullptr;
	olc::Decal* OptionsBGDecal = nullptr;
    olc::Decal* EnemyBulletDecal = nullptr;

    ETutorialStep tutorialStep = ETutorialStep::Intro;

    bool tutorialActive = true;
    bool tutorialStepStarted = false;

    std::vector<std::string> currentTutorialDialogue;
    std::string currentSpeakerName = "Academia";

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

    float _LobbyTime = 0.0f;

    std::vector<OptionsParticle> _OptionsParticles;
    float _OptionsTime = 0.0f;

    std::deque<std::unique_ptr<UIButton>> _Buttons;
};
