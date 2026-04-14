#include <Ennemies.h>
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"

void Ennemies::Draw(AcademiaEngine& engine) {
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
	engine.FillCircle(pixelPos, static_cast<int32_t>(Radius), Color);
}

olc::vf2d Ennemies::GetPlayerPosition(AcademiaEngine& engine, Player& player) {
	return player.Position;
}

void Ennemies::GoToPlayer(AcademiaEngine& engine, float force, olc::vf2d playerPosition) {
	AddForce(engine, force, playerPosition);
}

void Ennemies::AddForce(AcademiaEngine& engine, float force, olc::vf2d direction)
{
	Position += direction * force;
}