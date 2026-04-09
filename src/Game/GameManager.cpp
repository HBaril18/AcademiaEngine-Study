#include "GameManager.h"
#include "../../external/olc/olcPixelGameEngine.h"
#include "../Engine/AcademiaEngine.h"

void GameManager::Initialize(AcademiaEngine* engineContext)
{
    _EngineContext = engineContext;

#ifdef ACADEMIA_EXAMPLE
    _ExampleObject.Position = { 0.0f, 0.0f };
#endif
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
#ifdef ACADEMIA_EXAMPLE
    _ExampleObject.Update(elapsedTime);
    _ExampleObject.Draw(*_EngineContext);
#endif
}
