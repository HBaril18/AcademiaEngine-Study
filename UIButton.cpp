#include "UIButton.h"

UIButton::UIButton(AcademiaEngine* engine)
{
    _EngineContext = engine;
}

void UIButton::DrawMovingCornerLine(
    olc::vf2d fromCorner,
    olc::vf2d toCorner,
    bool startsLeft,
    bool endsLeft,
    bool startsTop,
    bool endsTop,
    float anim,
    float length,
    olc::Pixel color
)
{
    anim = Clamp(anim);

    float shrinkPhase = Clamp(anim / 0.25f);
    float movePhase = Clamp((anim - 0.25f) / 0.50f);
    float growPhase = Clamp((anim - 0.75f) / 0.25f);

    shrinkPhase = SmoothStep(shrinkPhase);
    movePhase = SmoothStep(movePhase);
    growPhase = SmoothStep(growPhase);

    // ------------------------------------------------
    // Calculate SAFE horizontal line positions
    // This prevents the line from exceeding the button.
    // ------------------------------------------------

    olc::vf2d startH1;
    olc::vf2d startH2;

    if (startsLeft)
    {
        startH1 = fromCorner;
        startH2 = { fromCorner.x + length, fromCorner.y };
    }
    else
    {
        startH1 = { fromCorner.x - length, fromCorner.y };
        startH2 = fromCorner;
    }

    olc::vf2d endH1;
    olc::vf2d endH2;

    if (endsLeft)
    {
        endH1 = toCorner;
        endH2 = { toCorner.x + length, toCorner.y };
    }
    else
    {
        endH1 = { toCorner.x - length, toCorner.y };
        endH2 = toCorner;
    }

    olc::vf2d currentH1 =
    {
        Lerp(startH1.x, endH1.x, movePhase),
        Lerp(startH1.y, endH1.y, movePhase)
    };

    olc::vf2d currentH2 =
    {
        Lerp(startH2.x, endH2.x, movePhase),
        Lerp(startH2.y, endH2.y, movePhase)
    };

    // Draw moving horizontal line
    _EngineContext->DrawLineDecal(currentH1, currentH2, color);

    // ------------------------------------------------
    // Vertical line shrinks at start, then grows at end
    // ------------------------------------------------

    float startVerticalDir = startsTop ? 1.0f : -1.0f;
    float endVerticalDir = endsTop ? 1.0f : -1.0f;

    if (anim < 0.25f)
    {
        float verticalLength = length * (1.0f  - shrinkPhase);

        if (verticalLength > 0.5f)
        {
            _EngineContext->DrawLineDecal(
                fromCorner,
                {
                    fromCorner.x,
                    fromCorner.y + verticalLength * startVerticalDir
                },
                color
            );
        }
    }
    else if (anim > 0.75f)
    {
        float verticalLength = length * growPhase;

        if (verticalLength > 0.5f)
        {
            _EngineContext->DrawLineDecal(
                toCorner,
                {
                    toCorner.x,
                    toCorner.y + verticalLength * endVerticalDir
                },
                color
            );
        }
    }
}

void UIButton::Update(float dt)
{
    float speedIn = 2.8f;
    float speedOut = 2.0f;

    if (OnHover())
        _ButtonLineAnim += dt * speedIn;
    else
        _ButtonLineAnim -= dt * speedOut;

    _ButtonLineAnim = std::clamp(_ButtonLineAnim, 0.0f, 1.0f);
}

bool UIButton::OnHover()
{
	return _EngineContext->GetMouseX() >= _Position.x &&
		_EngineContext->GetMouseX() <= _Position.x + _Size.x &&
		_EngineContext->GetMouseY() >= _Position.y &&
		_EngineContext->GetMouseY() <= _Position.y + _Size.y;
}

void UIButton::Draw()
{
    bool hover = OnHover();
    float t = _ButtonLineAnim;

    olc::Pixel purple = olc::Pixel(170, 45, 255);
    olc::Pixel darkPurple = olc::Pixel(55, 15, 95);
    olc::Pixel deepPurple = olc::Pixel(22, 10, 42);
    olc::Pixel white = olc::Pixel(235, 220, 255);
    olc::Pixel glowPurple = olc::Pixel(220, 90, 255);
    olc::Pixel shadow = olc::Pixel(0, 0, 0, 120);
    olc::Pixel textShadow = olc::Pixel(20, 5, 35, 180);

    olc::vf2d pos = _Position;
    olc::vf2d size = _Size;

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
    // Animated corner lines
    // -------------------------------------------------
    float inset = 10.0f;
    float cornerLength = 18.0f;

    olc::Pixel lineColor = hover
        ? olc::Pixel(255, 230, 255)
        : olc::Pixel(220, 190, 255);

    DrawMovingCornerLine(
        { pos.x + inset, pos.y + inset },
        { pos.x + size.x - inset, pos.y + inset },
        true,
        false,
        true,
        true,
        t,
        cornerLength,
        lineColor
    );

    DrawMovingCornerLine(
        { pos.x + size.x - inset, pos.y + size.y - inset },
        { pos.x + inset, pos.y + size.y - inset },
        false,
        true,
        false,
        false,
        t,
        cornerLength,
        lineColor
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