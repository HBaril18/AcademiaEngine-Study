#include "UISlider.h"

UISlider::UISlider(AcademiaEngine* engine)
{
    _EngineContext = engine;
}

bool UISlider::OnHover()
{
    return _EngineContext->GetMouseX() >= _Position.x &&
        _EngineContext->GetMouseX() <= _Position.x + _Size.x &&
        _EngineContext->GetMouseY() >= _Position.y &&
        _EngineContext->GetMouseY() <= _Position.y + _Size.y;
}

void UISlider::Draw(){
    olc::Pixel purple = olc::Pixel(170, 45, 255);
    olc::Pixel darkPurple = olc::Pixel(55, 15, 95);
    olc::Pixel deepPurple = olc::Pixel(22, 10, 42);
    olc::Pixel white = olc::Pixel(235, 220, 255);
    olc::Pixel glowPurple = olc::Pixel(220, 90, 255);
    olc::Pixel shadow = olc::Pixel(0, 0, 0, 120);
    olc::Pixel textShadow = olc::Pixel(20, 5, 35, 180);

    olc::vf2d pos = _Position;
    olc::vf2d size = _Size;

    bool hover = OnHover();

    // -------------------------------------------------
    // Shadow / depth
    // -------------------------------------------------
    _EngineContext->FillRectDecal(
        pos + olc::vf2d(4.0f, 5.0f),
        size,
        shadow
    );

    // -------------------------------------------------
    // Outer hover glow
    // -------------------------------------------------
    if (hover)
    {
        _EngineContext->DrawRectDecal(
            pos - olc::vf2d(3.0f, 3.0f),
            size + olc::vf2d(6.0f, 6.0f),
            olc::Pixel(220, 90, 255, 80)
        );

        _EngineContext->DrawRectDecal(
            pos - olc::vf2d(2.0f, 2.0f),
            size + olc::vf2d(4.0f, 4.0f),
            olc::Pixel(220, 90, 255, 120)
        );
    }

    // -------------------------------------------------
    // Main background base
    // -------------------------------------------------
    _EngineContext->FillRectDecal(pos, size, deepPurple);

    // -------------------------------------------------
    // Fake vertical gradient
    // -------------------------------------------------
    for (int y = 0; y < int(size.y); y++)
    {
        float k = float(y) / size.y;

        int r = int(70 + (120 * (1.0f - k)));
        int g = int(20 + (25 * (1.0f - k)));
        int b = int(120 + (90 * (1.0f - k)));

        if (hover)
        {
            r += 25;
            b += 35;
        }

        _EngineContext->FillRectDecal(
            { pos.x, pos.y + float(y) },
            { size.x, 1.0f },
            olc::Pixel(r, g, b)
        );
    }

    // -------------------------------------------------
    // Top highlight strip
    // -------------------------------------------------
    _EngineContext->FillRectDecal(
        pos + olc::vf2d(2.0f, 2.0f),
        { size.x - 4.0f, 2.0f },
        olc::Pixel(255, 210, 255, hover ? 170 : 110)
    );

    // -------------------------------------------------
    // Bottom depth strip
    // -------------------------------------------------
    _EngineContext->FillRectDecal(
        { pos.x + 2.0f, pos.y + size.y - 4.0f },
        { size.x - 4.0f, 2.0f },
        olc::Pixel(20, 5, 40, 180)
    );

    // -------------------------------------------------
    // Border
    // -------------------------------------------------
    _EngineContext->DrawRectDecal(
        pos,
        size,
        hover ? glowPurple : white
    );

    // Inner dark border, gives a bevel feel
    _EngineContext->DrawRectDecal(
        pos + olc::vf2d(1.0f, 1.0f),
        size - olc::vf2d(2.0f, 2.0f),
        olc::Pixel(30, 8, 55, 140)
    );

    // -------------------------------------------------
    // Text
    // -------------------------------------------------
    std::string text = _Text;

    if (_TextProvider)
        text = _TextProvider();

    olc::vi2d textSize = _EngineContext->GetTextSize(text);

    olc::vf2d textPos =
    {
        pos.x + size.x / 2.0f - textSize.x * _TextScale.x * 0.5f,
        pos.y + size.y / 2.0f - textSize.y * _TextScale.y * 0.5f
    };

    // Text shadow
    _EngineContext->DrawStringDecal(
        textPos + olc::vf2d(2.0f, 2.0f),
        text,
        textShadow,
        _TextScale
    );

    // Main text
    _EngineContext->DrawStringDecal(
        textPos,
        text,
        hover ? olc::Pixel(255, 245, 255) : white,
        _TextScale
    );
}