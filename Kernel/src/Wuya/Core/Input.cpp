#include "pch.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include <Wuya/Application/Application.h>

namespace Wuya
{
	bool Input::IsKeyPressed(const KeyCode key)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Instance()->GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(const MouseCode btn)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Instance()->GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(btn));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePos()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Instance()->GetWindow().GetNativeWindow());
		double pos_x, pos_y;
		glfwGetCursorPos(window, &pos_x, &pos_y);

		return glm::vec2(static_cast<float>(pos_x), static_cast<float>(pos_y));
	}

	float Input::GetMouseX()
	{
		return GetMousePos().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePos().y;
	}
}