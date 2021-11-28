#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"
#include <glm/glm.hpp>

namespace Wuya
{
	class Input
	{
	public:
		static bool IsKeyPressed(const KeyCode key);

		static bool IsMouseButtonPressed(const MouseCode btn);
		static glm::vec2 GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}