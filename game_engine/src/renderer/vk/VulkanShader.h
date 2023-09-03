#pragma once
#include "pch.h"
#include "VulkanTypes.h"
#include "vulkan/vulkan.h"

namespace vk {
	class VulkanShader {
	public:
		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;

		VulkanShader(const VulkanContext& context, const std::string& vert_filename, const std::string& frag_filename);
		static std::vector<char> ReadFile(const std::string& filename);
		static VkShaderModule CreateShaderModule(const std::vector<char>& code, const VulkanContext& context);
	private:
		std::string m_vert_filepath;
		std::string m_frag_filepath;
	};

}

