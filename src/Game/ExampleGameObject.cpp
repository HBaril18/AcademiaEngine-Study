#include "ExampleGameObject.h"
#include "../Engine/AcademiaEngine.h"

void ExampleGameObject::Draw(AcademiaEngine& engine)
{
    olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
    engine.FillCircle(pixelPos, static_cast<int32_t>(Radius), Color);
    
    //convert avant le draw
}
