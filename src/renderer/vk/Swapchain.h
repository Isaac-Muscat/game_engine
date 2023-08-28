#pragma once
#include "Core.h"
#include "Types.h"
#include "hid/Window.h"
#include <vulkan/vulkan.h>


namespace vk {
	class Swapchain {
	public:
		Swapchain(const VulkanContext& context);

		VkSwapchainKHR mSwapchain;
		SwapChainSupportDetails mSwapChainSupportDetails;
		VkSurfaceFormatKHR mSurfaceFormat;
		VkPresentModeKHR mPresentMode;
		VkExtent2D mExtent;
		std::vector<VkImage> mSwapchainImages;
		std::vector<VkImageView> mSwapchainImageViews;
		std::vector<VkFramebuffer> mSwapchainFramebuffers;
	};
}
