#pragma once
#include "pch.h"
#include "Image.h"


namespace vk {
	class Texture {
	public:
		std::shared_ptr<Image> m_image;

		Texture(std::string filepath, const VulkanContext& context);
		void transition_image_layout(const VulkanContext& context, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
		void generate_mipmaps(const VulkanContext& context, VkFormat imageFormat, int32_t texWidth, int32_t texHeight);
		void copy_buffer_to_image(const VulkanContext& context, VkBuffer buffer, uint32_t width, uint32_t height);
		void create_sampler(const VulkanContext& context);
	private:
		uint32_t m_miplevels;
		VkSampler m_sampler;
	};
}