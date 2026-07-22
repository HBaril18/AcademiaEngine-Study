#pragma once
#include <olc/olcPixelGameEngine.h>
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include <functional>

class AcademiaEngine;

class UIButton
{
public:
    UIButton(AcademiaEngine* engine);
    void Update(float dt);
    void Draw();
	bool OnHover();

	//CALLBACK FOR BUTTON CLICK EVENT
    void SetOnClick(const std::function<void()>& callback)
    {
        _OnClick = callback;
    }
    void ExecuteOnClick()
    {
        if (_OnClick)
        {
            _OnClick();
        }
    }

	void SetPosition(const olc::vf2d& position) { _Position = position; }
	void SetSize(const olc::vf2d& size) { _Size = size; }
	void SetText(const std::string& text) { _Text = text; }
	void SetTextScale(const olc::vf2d& scale) { _TextScale = scale; }

    olc::vf2d GetPosition() const { return _Position; }
	olc::vf2d GetSize() const { return _Size; }
	std::string GetText() const { return _Text; }
    olc::vf2d GetTextScale() const { return _TextScale; }

    void DrawMovingCornerLine(
        olc::vf2d fromCorner,
        olc::vf2d toCorner,
        bool startsLeft,
        bool endsLeft,
        bool startsTop,
        bool endsTop,
        float anim,
        float length,
        olc::Pixel color);

    float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }
    float Clamp(float v)
    {
        if (v < 0.0f) return 0.0f;
        if (v > 1.0f) return 1.0f;
        return v;
    }
    float SmoothStep(float t)
    {
        t = Clamp(t);
        return t * t * (3.0f - 2.0f * t);
    }

    void SetTextProvider(std::function<std::string()> provider)
    {
        _TextProvider = std::move(provider);
    }

private:
	olc::vf2d _Position;
	olc::vf2d _Size;
	std::string _Text;
    olc::vf2d _TextScale;

	float _HoverAnimation = 0.0f;
    float _ButtonLineAnim = 0.0f;

    AcademiaEngine* _EngineContext = nullptr;

    std::function<void()> _OnClick;
    std::function<std::string()> _TextProvider;
};