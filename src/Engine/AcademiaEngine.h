// ReSharper disable All
#pragma once

#include "../../external/olc/olcPixelGameEngine.h"
#include "../Utils.h"

class GameManager;

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

private:

    void PreUpdate(float elapsedTime);
    void Update(float elapsedTime);
    void PostUpdate(float elapsedTime);

    GameManagerPtr _GameManager = nullptr;
};
