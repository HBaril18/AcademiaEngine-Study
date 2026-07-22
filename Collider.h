#pragma once
#include "../AcademiaEngine-Study/src/Game/GameObject.h"


	struct Collider
	{
		GameObject* owner;
		enum class EColliderType { Circle, Box } type;
		olc::vf2d position;
		float size; // For circle: radius, for box: half-size
		std::uint32_t layer; // For collision filtering
		bool enabled;
	};