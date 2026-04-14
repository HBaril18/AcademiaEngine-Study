#include <Ennemies.h>
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"

void Ennemies::Draw(AcademiaEngine& engine) {
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
	engine.FillCircle(pixelPos, static_cast<int32_t>(Radius), Color);
}

olc::vf2d GetPlayerPosition(AcademiaEngine& engine, Player& player) {
	if (&player) {
		return player.Position;
	}
	return {};
}

void GoToPlayer(AcademiaEngine& engine, float force, olc::vf2d playerPosition) {
	AddForce(engine, force, playerPosition);
}