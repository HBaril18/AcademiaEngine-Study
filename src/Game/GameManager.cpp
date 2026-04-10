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
	constexpr olc::Key shiftKey = olc::Key::SHIFT;
    const olc::HWButton moveRightButton = _EngineContext->GetKey(rightKey);
    const olc::HWButton moveLeftButton = _EngineContext->GetKey(leftKey);
    const olc::HWButton moveUpButton = _EngineContext->GetKey(upKey);
    const olc::HWButton moveDownButton = _EngineContext->GetKey(downKey);
    const olc::HWButton jumpButton = _EngineContext->GetKey(spaceKey);
	const olc::HWButton sneakButton = _EngineContext->GetKey(shiftKey);
    
    // Gameplay code
    //Movement handle
    float x = 0.0f;
    float y = 0.0f;

    if (moveRightButton.bHeld) { x += 1.0f; }
    if (moveLeftButton.bHeld)  { x -= 1.0f; }
    if (moveUpButton.bHeld)    { y += 1.0f; }
    if (moveDownButton.bHeld)  { y -= 1.0f; }
	if (sneakButton.bHeld) { x *= 0.2f; y *= 0.2f; }

    // Normalize diagonal movement
    if (x != 0.0f && y != 0.0f) {
        x *= 0.5f;
        y *= 0.5f;
    }

    std::vector<float> direction = { x, y };

    _Player.AddForce(*_EngineContext, 0.7, direction);
	_Player.DrawCursor(*_EngineContext, _Player.GetCursorPosition(*_EngineContext));
#ifdef ACADEMIA_EXAMPLE
    _Player.Update(elapsedTime);
    _Player.Draw(*_EngineContext);
#endif
}

void GameManager::Uninitialize() {
    //detruit les new et pointeur que j'ai créer
}