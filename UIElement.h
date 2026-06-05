#pragma once
#include "external/olc/olcPixelGameEngine.h"

class UIElement
{
	public :
		virtual void Update(float dt);
		virtual void Render();
		virtual void OnMouseDown(const olc::vi2d& mousePos);
		virtual void OnMouseUp(const olc::vi2d& mousePos);

	protected:
		olc::vf2d position; 
		olc::vf2d size;
		bool visible = true; 
		bool enabled = true;

	private:
		// Helper function to check if a point is within the bounds of the UI element
		bool Contains(const olc::vi2d point) { return point.x >= position.x && point.x < position.x + size.x && point.y >= position.y && point.y < position.y + size.y; }
};