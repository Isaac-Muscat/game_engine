#include "pch.h"
#include "VulkanShader.h"

namespace vk {
	VulkanShader::VulkanShader(const VulkanContext& context, const std::string& vert_filepath, const std::string& frag_filepath) 
		: m_vert_filepath(vert_filepath), m_frag_filepath(frag_filepath) {
		auto vertShaderCode = ReadFile(vert_filepath);
		auto fragShaderCode = ReadFile(frag_filepath);

		m_vert_shader_module = CreateShaderModule(vertShaderCode, context);
		m_frag_shader_module = CreateShaderModule(fragShaderCode, context);
	}
	std::vector<char> VulkanShader::ReadFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::vector<char>& code, const VulkanContext& context) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(context.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}
}
