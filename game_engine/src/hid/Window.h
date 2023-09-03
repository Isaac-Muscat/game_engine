#pragma once
#include "pch.h"

#include "vulkan/vulkan.h"

class Window {
public:
	virtual bool ShouldClose() = 0;
	virtual void PollEvents() = 0;
	virtual std::vector<const char*> GetRequiredExtensions(bool enable_validation_layers) const = 0;
	virtual void CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* p_surface) const = 0;
	virtual void GetFrameBufferSize(int* width, int* height) = 0;
};

extern std::unique_ptr<Window> g_window;
