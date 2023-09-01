#pragma once
#include "pch.h"
#include "Types.h"
#include "vulkan/vulkan.h"

namespace vk {
	class Image {
	public:
		VkImageView m_view;
		Image(const VulkanContext& context, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, 
			VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect_flags, VkMemoryPropertyFlags properties);
		~Image();

	private:
		const VulkanContext& m_context;
		VkImage m_image;
		VkDeviceMemory m_memory;
	};
}