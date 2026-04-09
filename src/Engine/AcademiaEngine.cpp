#include "AcademiaEngine.h"

#include "../Game/GameManager.h"

static AcademiaEngine* _Instance = nullptr;

AcademiaEngine::AcademiaEngine()
{
    sAppName = "AcademiaEngine";
}

bool AcademiaEngine::OnUserCreate()
{
    _GameManager = std::make_shared<GameManager>();
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
    if(_GameManager)
        _GameManager->Update(elapsedTime);
}

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
