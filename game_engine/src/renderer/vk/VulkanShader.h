#pragma once
#include "pch.h"
#include "VulkanTypes.h"
#include "vulkan/vulkan.h"

namespace vk {
	class VulkanShader {
	public:
		VkShaderModule m_shader_module;
		std::string m_filepath;

		VulkanShader(const VulkanContext& context, const std::string& filename);
		static std::vector<char> ReadFile(const std::string& filename);
		static VkShaderModule CreateShaderModule(const std::vector<char>& code, const VulkanContext& context);
	};

    struct ShaderStages {
        std::shared_ptr<VulkanShader> vertex_shader;
        std::shared_ptr<VulkanShader> fragment_shader;
    };

}

