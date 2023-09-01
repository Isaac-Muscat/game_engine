#pragma once
#include "pch.h"
#define GLFW_INCLUDE_VULKAN
#include "hid/Window.h"

#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

class WindowsWindow : public Window {
public:
	uint32_t mWidth;
	uint32_t mHeight;
	std::string mName;
public:
	WindowsWindow(int width, int height, std::string name);
	~WindowsWindow();
	bool should_close() override { return glfwWindowShouldClose(mWindow); }
	void poll_events() override { return glfwPollEvents(); }
	std::vector<const char*> get_required_extensions(bool enableValidationLayers) const;
	void create_vulkan_surface(VkInstance instance, VkSurfaceKHR* pSurface) const;
	void get_frame_buffer_size(int* width, int* height);
private:
	GLFWwindow* mWindow;
};