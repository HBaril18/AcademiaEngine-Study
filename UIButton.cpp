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
    if (OnHover())
        _ButtonLineAnim += dt;
    else
        _ButtonLineAnim -= dt;

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
    olc::Pixel purple = olc::Pixel(170, 45, 255);       // Main neon purple
    olc::Pixel darkPurple = olc::Pixel(75, 20, 120);    // Dark border / inactive state
    olc::Pixel white = olc::Pixel(235, 220, 255);       // Soft violet-white highlight
    olc::Pixel dark = olc::Pixel(8, 6, 18);             // Deep space background
    olc::Pixel lightPurple = olc::Pixel(220, 90, 255);  // Hover / glow color

    // Hover highlight
    if (OnHover())
    {
		purple = lightPurple;
        _EngineContext->FillRectDecal(
            _Position,
            { _Size.x, _Size.y },
            lightPurple
        );
    }

    // Main body
    _EngineContext->FillRectDecal(
        _Position,
        _Size,
        purple
    );

    // Border
    _EngineContext->DrawRectDecal(
        _Position,
        _Size,
        white
    );

    float t = _ButtonLineAnim;

    float inset = 10.0f;
    float cornerLength = 18.0f;

    // Top-left goes to top-right
    DrawMovingCornerLine(
        { float(_Position.x + inset), float(_Position.y + inset) },
        { float(_Position.x + _Size.x - inset), float(_Position.y + inset) },
        true,   // starts left
        false,  // ends right
        true,   // starts top
        true,   // ends top
        t,
        cornerLength,
        white
    );

    // Bottom-right goes to bottom-left
    DrawMovingCornerLine(
        { float(_Position.x + _Size.x - inset), float(_Position.y + _Size.y - inset) },
        { float(_Position.x + inset),     float(_Position.y + _Size.y - inset) },
        false,  // starts right
        true,   // ends left
        false,  // starts bottom
        false,  // ends bottom
        t,
        cornerLength,
        white
    );

    std::string text = _Text;

    if (_TextProvider)
        text = _TextProvider();

    _EngineContext->DrawStringDecal(
        { float(_Position.x + _Size.x / 2 - _EngineContext->GetTextSize(text).x * _TextScale.x * 0.5f),
        float(_Position.y + _Size.y / 2 - _EngineContext->GetTextSize(text).y * _TextScale.y * 0.5f) },
        text,
        white,
        _TextScale
    );
}