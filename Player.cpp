#include "Player.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"
#include "Ennemies.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

void Player::Draw(AcademiaEngine& engine) { //Draw the player as a circle and a barrel pointing towards the cursor
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
	engine.FillCircle(pixelPos, static_cast<int32_t>(Radius), Color); //Player core
	olc::vf2d direction = GetPlayerDirection(engine); //Cursor direction
	direction = direction.norm();
	float length = Radius + 20; //Length of the barrell
	olc::vf2d endWorldPos = Position + direction * length;
	olc::vi2d endPixelPos = engine.ConvertWorldPositionToPixels(endWorldPos);
	engine.DrawLine(pixelPos.x, pixelPos.y, endPixelPos.x, endPixelPos.y, Color);
}

void Player::SpawnBullet(AcademiaEngine& engine) {
	Bullet bullet = Bullet(); // Create a new bullet instance
	bullet.Position = Position; // Set the bullet's initial position to the player's position 
	olc::vf2d direction = GetPlayerDirection(engine);
	if (direction.mag() > 0.0f)
		direction = direction.norm();
	else
		direction = { 1.0f, 0.0f }; // default direction
	bullet.SetDirection(direction);
	bullets.push_back(bullet); // Add the bullet to the player's bullet list
	std::cout << "Bullet deque" << bullets.size() << std::endl;
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
		}
	}
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
}

olc::vf2d Player::GetCursorPosition(AcademiaEngine& engine) {
	olc::vi2d cursorPixelPos = engine.GetMousePos();
	olc::vf2d cursorWorldPos = engine.ConvertPixelsToWorldPosition(cursorPixelPos);
	return cursorWorldPos;
}