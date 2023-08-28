#pragma once
#include "Core.h"
#include "vulkan/vulkan.h"

class Window {
public:
	virtual bool should_close() = 0;
	virtual void poll_events() = 0;
	virtual std::vector<const char*> get_required_extensions(bool enableValidationLayers) const = 0;
	virtual void create_vulkan_surface(VkInstance instance, VkSurfaceKHR* pSurface) const = 0;
	virtual void get_frame_buffer_size(int* width, int* height) = 0;
};

extern std::unique_ptr<Window> g_window;
