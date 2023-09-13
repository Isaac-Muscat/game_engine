#pragma once
#include "pch.h"
#include "VulkanTypes.h"
#include "VulkanImage.h"
#include "hid/Window.h"
#include <vulkan/vulkan.h>


namespace vk {
	class VulkanSwapchain {
	public:
		VulkanSwapchain(const VulkanContext& context, VkRenderPass renderpass);
		void GetNextImage(uint32_t current_frame);
		VkFramebuffer GetCurrentFramebuffer() { return m_framebuffers[m_image_index]; }
		void Submit(const std::vector<VkCommandBuffer>& submitCommandBuffers, uint32_t current_frame);
		uint32_t m_image_index;
		std::vector<VkSemaphore> m_image_available_semaphores;
		std::vector<VkSemaphore> m_render_finished_semaphores;
		std::vector<VkFence> m_in_flight_fences;

		VkSwapchainKHR m_swapchain;
		SwapChainSupportDetails m_swapchain_details;
		VkSurfaceFormatKHR m_surface_format;
		VkPresentModeKHR m_present_mode;
		VkExtent2D m_extent;

		std::shared_ptr<VulkanImage> m_color_image;
		std::shared_ptr<VulkanImage> m_depth_image;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_image_views;
		std::vector<VkFramebuffer> m_framebuffers;
	};
}
