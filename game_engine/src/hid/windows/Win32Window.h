#pragma once
#include "pch.h"
#include "hid/Window.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"

namespace win32 {
	class Win32Window : public Window {
	public:
		uint32_t m_width;
		uint32_t m_height;
		std::string m_name;
	public:
		Win32Window(int width, int height, std::string name);
		~Win32Window();
		bool ShouldClose() override { return glfwWindowShouldClose(m_window); }
		void PollEvents() override { return glfwPollEvents(); }
		std::vector<const char*> GetRequiredExtensions(bool enable_validation_layers) const override;
		void CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* p_surface) const override;
		void GetFrameBufferSize(int* width, int* height) override;
		void* GetNativeWindowPointer() override;
	private:
		GLFWwindow* m_window;
	};
}
