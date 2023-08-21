#pragma once
#include "Core.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

class Window {
public:
	uint32_t mWidth;
	uint32_t mHeight;
	std::string mName;
public:
	Window(int width, int height, std::string name);
	~Window();
	bool ShouldClose() { return glfwWindowShouldClose(mWindow); }
	void PollEvents() { return glfwPollEvents(); }
	std::vector<const char*> GetRequiredExtensions(bool enableValidationLayers) const;
	void CreateSurface(VkInstance instance, VkSurfaceKHR* pSurface) const;
	void GetFramebufferSize(int* width, int* height);
private:
	GLFWwindow* mWindow;
};
