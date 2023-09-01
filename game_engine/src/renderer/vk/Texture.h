#pragma once
#include "pch.h"
#include "Image.h"


namespace vk {
	class Texture {
	public:
		std::shared_ptr<Image> m_image;

		Texture(std::string filepath, const VulkanContext& context);
	private:
		uint32_t m_miplevels;
	};
}