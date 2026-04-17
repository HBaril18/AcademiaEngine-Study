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
	
	// You are declaring a function that has different parameters that AddForce from the parent. 
	// Maybe add the new AddForce to the parent as well, virtual.
	// Also the direction shoud not be a copy. Use a const& if you can
    void AddForce(AcademiaEngine& engine, float force, const std::vector<float>& direction);
	// I suppose that what you did is fine, but confusing :) 
    void AddForce(AcademiaEngine& engine) override { /* Not used — Player movement goes through AddForce(engine, force, direction) */ }
	
	// Try as much as you can to make your Accessors (Get...) const.
	// This is very good way to document your code, to explain that the intention is to return a value, 
	// and that the function is not supposed to change your Player in any way.
    olc::vf2d GetCursorPosition(AcademiaEngine& engine) const;
	olc::vf2d GetPlayerDirection(AcademiaEngine& engine) const;
    void SpawnBullet(AcademiaEngine& engine);
	
// Consider making these protected — public fields can be written from anywhere,
// which makes bugs harder to track down. Encapsulation is your friend here :)
// protected:
	
	// Good initialization :) 
    float Radius = 20.0f;
    olc::Pixel Color = olc::WHITE;
	
	// Try to establish your own naming convention, and hold onto it. 
	// Radius R is capital, probably bullets b should be capital too.
    std::deque<Bullet> bullets;
};
