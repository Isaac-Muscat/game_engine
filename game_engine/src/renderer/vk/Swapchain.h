#pragma once
#include "pch.h"
#include "Types.h"
#include "Image.h"
#include "hid/Window.h"
#include <vulkan/vulkan.h>


namespace vk {
	class Swapchain {
	public:
		Swapchain(const VulkanContext& context, VkRenderPass renderpass);

		VkSwapchainKHR mSwapchain;
		SwapChainSupportDetails mSwapChainSupportDetails;
		VkSurfaceFormatKHR mSurfaceFormat;
		VkPresentModeKHR mPresentMode;
		VkExtent2D mExtent;

		std::shared_ptr<Image> m_color_image;
		std::shared_ptr<Image> m_depth_image;
		std::vector<VkImage> mSwapchainImages;
		std::vector<VkImageView> mSwapchainImageViews;
		std::vector<VkFramebuffer> mSwapchainFramebuffers;
	};
}
