#include "GameManager.h"
#include "../../external/olc/olcPixelGameEngine.h"
#include "../Engine/AcademiaEngine.h"

void GameManager::Initialize(AcademiaEngine* engineContext)
{
    _EngineContext = engineContext;

#ifdef ACADEMIA_EXAMPLE
    _Player.Position = { 0.0f, 0.0f };
#endif

    //faire bouger la sphere (class playercharacter + characterMovement) 
    //ennemie + bullet
    //newworld (scene avec objet) plus vers la fin
}

void GameManager::Update(float elapsedTime)
{
    constexpr olc::Key rightKey = olc::Key::D;
    constexpr olc::Key leftKey = olc::Key::A;
    constexpr olc::Key upKey = olc::Key::W;
    constexpr olc::Key downKey = olc::Key::S;
    constexpr olc::Key spaceKey = olc::Key::SPACE;
    const olc::HWButton moveRightButton = _EngineContext->GetKey(rightKey);
    const olc::HWButton moveLeftButton = _EngineContext->GetKey(leftKey);
    const olc::HWButton moveUpButton = _EngineContext->GetKey(upKey);
    const olc::HWButton moveDownButton = _EngineContext->GetKey(downKey);
    const olc::HWButton jumpButton = _EngineContext->GetKey(spaceKey);
    
    // Gameplay code
    std::vector<float> direction;
    if (moveRightButton.bPressed) {
        direction = { 1.0, 0.0 };
    }
    else if (moveLeftButton.bPressed) {
        direction = { -1.0, 0.0 };
    }
    else if (moveUpButton.bPressed) {
        direction = { 0.0, 1.0 };
    }
    else if (moveDownButton.bPressed) {
        direction = { 0.0, -1.0 };
    }
    else {
        direction = { 0.0, 0.0 };
    }

    _Player.AddForce(*_EngineContext, 10, direction);
#ifdef ACADEMIA_EXAMPLE
    _Player.Update(elapsedTime);
    _Player.Draw(*_EngineContext);
#endif
}

void GameManager::Uninitialize() {
    //detruit les new et pointeur que j'ai créer
}