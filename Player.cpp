#include "Player.h"
#include "../AcademiaEngine-Study/src/Engine/AcademiaEngine.h"

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

void Player::Update(AcademiaEngine& engine, float elapsedTime) {
	UpdateBullets(engine);
	if (damageCooldown > 0.0f) {
		damageCooldown -= elapsedTime;
	}
}

void Player::Draw(AcademiaEngine& engine) 
{
	const olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
	engine.FillCircle(pixelPos, static_cast<int32_t>(Radius), Color); //Player core
	
	olc::vf2d direction = GetPlayerDirection(engine); //Cursor direction
	direction = direction.norm();
	
	const float length = Radius + 20; //Length of the barrel
	
	const olc::vf2d endWorldPos = Position + direction * length;
	const olc::vi2d endPixelPos = engine.ConvertWorldPositionToPixels(endWorldPos);
	
	engine.DrawLine(pixelPos.x, pixelPos.y, endPixelPos.x, endPixelPos.y, Color);
}

void Player::SpawnBullet(AcademiaEngine& engine) {
	Bullet bullet = Bullet(); // Create a new bullet instance
	bullet.SetPosition(Position); // Set the bullet's initial position to the player's position 
	
	olc::vf2d direction = GetPlayerDirection(engine);
	
	if (direction.mag() > 0.0f)
		direction = direction.norm();
	else
		direction = { 1.0f, 0.0f }; // default direction
	bullet.SetDirection(direction);
	bullets.push_back(bullet); // Add the bullet to the player's bullet list
	//std::cout << "Bullet deque" << bullets.size() << std::endl;
}

olc::vf2d Player::GetPlayerDirection(AcademiaEngine& engine) const {
	olc::vf2d cursorWorldPos = GetCursorPosition(engine);
	olc::vf2d direction = cursorWorldPos - Position;
	return direction;
}

void Player::DrawCursor(AcademiaEngine& engine, olc::vf2d cursorWorldPos) {
	olc::vi2d cursorPixelPos = engine.ConvertWorldPositionToPixels(cursorWorldPos);
	engine.DrawCircle(cursorPixelPos, 5, olc::RED);
}

void Player::AddForce(AcademiaEngine& engine, float force, const std::vector<float>& direction, float elapsedTime) {
	for (int i = 0; i < direction.size(); i++) {
		Position.x += direction[0] * force * elapsedTime;
		Position.y += direction[1] * force * elapsedTime;
	}
	olc::vi2d pixelPos = engine.ConvertWorldPositionToPixels(Position);
}

olc::vf2d Player::GetCursorPosition(AcademiaEngine& engine) const {
	// This will trigger a copy of the returned value from `engine.GetMousePos()`. 
	// cursorPixedPos should be a constant reference (const&) to prevent the copy of the vi2d
	const olc::vi2d& cursorPixelPos = engine.GetMousePos();
	
	// Here the copy is fine because ConvertPixelsToWorldPosition will return a copy
	olc::vf2d cursorWorldPos = engine.ConvertPixelsToWorldPosition(cursorPixelPos);
	return cursorWorldPos;
}

void Player::UpdateBullets(AcademiaEngine& engineContext) {
	bullets.erase( // taken from ChatGPT (to remove bullets that are out of the screen)
		std::remove_if(bullets.begin(), bullets.end(),
			[&](const Bullet& bullet) {
				olc::vi2d pixel = engineContext.ConvertWorldPositionToPixels(bullet.GetPosition());

				return pixel.x < 0 || pixel.x > engineContext.ScreenWidth() ||
					pixel.y < 0 || pixel.y > engineContext.ScreenHeight();
			}),
		bullets.end()
	);
}