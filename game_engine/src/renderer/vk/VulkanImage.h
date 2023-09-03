#pragma once
#include "pch.h"
#include "VulkanTypes.h"
#include "vulkan/vulkan.h"

namespace vk {
	class VulkanImage {
	public:
		VkImage m_image;
		VkImageView m_view;
		VulkanImage(const VulkanContext& context, uint32_t width, uint32_t height, uint32_t mip_levels, VkSampleCountFlagBits num_samples, VkFormat format, 
			VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect_flags, VkMemoryPropertyFlags properties);
		~VulkanImage();
	private:
		const VulkanContext& m_context;
		VkDeviceMemory m_memory;
	};
}