#pragma once
#include "pch.h"
#include "VulkanImage.h"


namespace vk {
	class VulkanTexture {
	public:
		std::shared_ptr<VulkanImage> m_image;
		VkSampler m_sampler;
		VulkanTexture(std::string filepath, const VulkanContext& context);
		void TransitionImageLayout(const VulkanContext& context, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
		void GenerateMipmaps(const VulkanContext& context, VkFormat image_format, int32_t tex_width, int32_t tex_height);
		void CopyBufferToImage(const VulkanContext& context, VkBuffer buffer, uint32_t width, uint32_t height);
		void CreateSampler(const VulkanContext& context);
	private:
		uint32_t m_miplevels;
	};
}