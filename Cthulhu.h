#pragma once
#include "Ennemies.h"

enum class CthulhuMovementState
{
    Intro,
    Orbit,
    Sweep,
    Teleport,
    Figure8,
    Summon
};

class Cthulhu : public Ennemies
{
public:
    Cthulhu(olc::vf2d pos);

    void SetGameManager(GameManager* gm) { gameManager = gm; }

    virtual void Update(AcademiaEngine& engine, float elapsedTime) override;
    virtual void Draw(AcademiaEngine& engine) override;

    void UpdateIntro(float elapsedTime, AcademiaEngine& engine);
	void UpdateOrbit(float elapsedTime);
	void UpdateSweep(float elapsedTime, AcademiaEngine& engine);
	void UpdateTeleport(float elapsedTime);
	void UpdateSummons(float elapsedTime);
	void UpdateFigure8(float elapsedTime);

    bool IsDialogueActive() const override;
	void DrawDialogue(AcademiaEngine& engine) override;
    void UpdateDialogue(float elapsedTime);

    virtual float GetSpeed() const override { return 40.0f; }
    virtual float GetDamage() const override { return 25.0f; }

    olc::Sprite* GetSprite(AcademiaEngine& engine)
    {
        return &engine.EyeChtulhuSheet;
    }

    float DialogueTimer = 0.0f;

    olc::vf2d topCenterWorld;

private:
    CthulhuMovementState _MovementState = CthulhuMovementState::Intro;

    olc::vf2d _TargetIntroPosition;
    int _DialogueIndex;
    bool _IntroInitialized = false;
    bool _ShowDialogue;
    std::vector<std::string> _Dialogue =
    {
        "...",
        "I HAVE WATCHED YOU.",
        "THOUSANDS HAVE FALLEN.",
        "YOU ARE NO DIFFERENT.",
        "COME MORTAL."
    };
    float _CharacterTimer = 0.0f;
    int _VisibleCharacters = 0;

    float _MovementStateTimer = 0.0f;
    float _OrbitAngle = 0.0f;
    float _TeleportTimer = 0.0f;
    float _SummonTimer = 0.0f;
    float _SweepDirection = 1.0f;
    float _PatternTimer = 0.0f;

    float _LookAngle = 0.0f;
    olc::vf2d _PreviousPosition;

    int _CurrentFrame = 0;
    float _AnimationTimer = 0.0f;
    float _AnimationSpeed = 0.1f;
};