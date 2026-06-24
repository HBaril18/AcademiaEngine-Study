#pragma once
#include "src/Game/GameObject.h"

class PowerUp : public GameObject
{
public:
	void Update(AcademiaEngine& engine, float elapsedTime) override;
	void Draw(AcademiaEngine& engine) override;
private:
};