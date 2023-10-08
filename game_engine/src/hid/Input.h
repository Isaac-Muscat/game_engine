#pragma once
#include "pch.h"
#include "windows/Win32Window.h"

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

namespace Input {
	static bool GetKeyPressed(uint16_t keycode) {
		return (GLFW_PRESS == glfwGetKey((GLFWwindow*)g_window->GetNativeWindowPointer(), keycode));
	}

	static bool GetKeyReleased(uint16_t keycode) {
		return (GLFW_RELEASE == glfwGetKey((GLFWwindow*)g_window->GetNativeWindowPointer(), keycode));
	}

	static bool GetMousePressed(uint16_t button) {
		return (GLFW_PRESS == glfwGetMouseButton((GLFWwindow*)g_window->GetNativeWindowPointer(), button));
	}

	static bool GetMouseReleased(uint16_t button) {
		return (GLFW_RELEASE == glfwGetMouseButton((GLFWwindow*)g_window->GetNativeWindowPointer(), button));
	}

	static glm::vec2 GetMousePosition() {
		double x, y;
		glfwGetCursorPos((GLFWwindow*)g_window->GetNativeWindowPointer(), &x, &y);
		return { x, y };
	}
};