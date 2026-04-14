#include "Player.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include <vector>
#include "Ennemies.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

void Player::Draw(AcademiaEngine& engine) {
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
	engine.FillCircle(pixelPos, static_cast<int32_t>(Radius), Color); //Player core
	olc::vf2d direction = GetPlayerDirection(engine); //Cursor direction
	direction = direction.norm();
	float length = Radius + 20; //Length of the barrell
	olc::vf2d endWorldPos = Position + direction * length;
	olc::vi2d endPixelPos = engine.ConvertWorldPositionToPixels(endWorldPos);
	engine.DrawLine(pixelPos.x, pixelPos.y, endPixelPos.x, endPixelPos.y, Color);
}

olc::vf2d Player::GetPlayerDirection(AcademiaEngine& engine) {
	olc::vf2d cursorWorldPos = GetCursorPosition(engine);
	olc::vf2d direction = cursorWorldPos - Position;
	return direction;
}

void Player::DrawCursor(AcademiaEngine& engine, olc::vf2d cursorWorldPos) {
	olc::vi2d cursorPixelPos = engine.ConvertWorldPositionToPixels(cursorWorldPos);
	engine.DrawCircle(cursorPixelPos, 5, olc::RED);
}

void Player::AddForce(AcademiaEngine& engine, float force, std::vector<float> direction) {
	for (int i = 0; i < direction.size(); i++) {
		if (direction[i]) {
			Position.x += direction[0] * force;
			Position.y += direction[1] * force;
			//std::cout << "Nouvelle position : x -> " << Position.a()[0] << ", y -> " << Position.a()[1] << std::endl;
		}
	}
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
}

olc::vf2d Player::GetCursorPosition(AcademiaEngine& engine) {
	olc::vi2d cursorPixelPos = engine.GetMousePos();
	olc::vf2d cursorWorldPos = engine.ConvertPixelsToWorldPosition(cursorPixelPos);
	return cursorWorldPos;
}

void Player::SpawnBullet(AcademiaEngine& engine, Bullet* bullet) {
	bullet = new Bullet();
	bullet->Position = Position; // Set the bullet's initial position to the player's position
	bullet->Draw(engine); // Draw the bullet immediately after spawning
	olc::vf2d direction = GetPlayerDirection(engine);
	bullet->SetDirection(direction);
	std::cout << "Bullet Spawned" << std::endl;
}