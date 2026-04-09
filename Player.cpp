#include "Player.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include <vector>

void Player::Draw(AcademiaEngine& engine) {
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
	engine.FillCircle(pixelPos, static_cast<int32_t>(Radius), Color);
	engine.FillTriangle(pixelPos, pixelPos+10, pixelPos+20, Color);
}

void Player::AddForce(AcademiaEngine& engine, float force, std::vector<float> direction) {
	int counter = 0;
	for (float value : direction) {
		counter++;
		if (value) {
			Position += direction[counter] * force;
		}
	}
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
}