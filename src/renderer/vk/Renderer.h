#pragma once
#include "Core.h"
#include "hid/Window.h"
#include "renderer/vk/Swapchain.h"

#include "vulkan/vulkan.h"

namespace vk {
	// Singleton

	class Renderer {
	public:
		void init(const Window& window);
		void destroy();

	public:
		VkInstance mInstance;
		VkSurfaceKHR mSurface;
		VkPhysicalDevice mPhysicalDevice;
		VkDevice mDevice;
		VkQueue mGraphicsQueue;
		VkQueue mPresentQueue;
		std::unique_ptr<Swapchain> mSwapchain;
		
		VkSampleCountFlagBits mMsaaSamples;
		VkDebugUtilsMessengerEXT mDebugMessenger;
	};
	extern Renderer gRenderer;
}