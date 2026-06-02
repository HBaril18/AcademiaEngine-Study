#pragma once
#include "Character.h"
#include "Bullet.h"
#include <deque>

/*----------------------------------*/
//                                  //
// MADE BY HENRICK BARIL 2026-04-09 //
//                                  //
/*----------------------------------*/

// Moving all those files in the Game directory would make it easier to navigate. (Player - Ennemies - Bullet)
class Player : public Character
{
public:
    void Draw(AcademiaEngine& engine) override;
	void DrawCursor(AcademiaEngine& engine, olc::vf2d cursorWorldPos);
	void AddForce(AcademiaEngine& engine, float force, const std::vector<float>& direction, float elapsedTime);
    olc::vf2d GetCursorPosition(AcademiaEngine& engine) const;
	olc::vf2d GetPlayerDirection(AcademiaEngine& engine) const;
    void SpawnBullet(AcademiaEngine& engine);
	std::deque<Bullet>& GetBullets() { return bullets; }
	void Update(AcademiaEngine& engine, float elapsedTime);
	void UpdateBullets(AcademiaEngine& engineContext);

protected:
	
    float Radius = 20.0f;
    olc::Pixel Color = olc::WHITE;
    std::deque<Bullet> bullets;
};