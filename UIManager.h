#pragma once
#include "UIElement.h"
#include <vector>

class AcademiaEngine;

class UIManager
{
public:
	void Initialize(AcademiaEngine* engineContext);
	void Update(float dt);
	void Render();
	olc::vf2d GetMousePosition(AcademiaEngine& engine) const;

protected:
	std::vector<std::unique_ptr<UIElement>> _elements;

private:
	AcademiaEngine* _EngineContext = nullptr;

};