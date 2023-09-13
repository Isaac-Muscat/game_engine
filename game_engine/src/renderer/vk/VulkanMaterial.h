#pragma once
#include "pch.h"
#include "VulkanTexture.h"

namespace vk {
	// TODO: Add builder functionality.
	class VulkanMaterial {
	public:
		VulkanMaterial(std::shared_ptr<VulkanTexture> texture);
		VkDescriptorSet* GetDescriptorSet(size_t current_frame) {
			return  &m_descriptor_sets[current_frame];
		}
	private:
		std::shared_ptr<VulkanTexture> m_texture;
		std::vector<VkDescriptorSet> m_descriptor_sets;
	};
}