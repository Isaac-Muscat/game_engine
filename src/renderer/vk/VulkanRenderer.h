#pragma once
#include "Core.h"
#include "hid/Window.h"
#include "renderer/Renderer.h"
#include "Swapchain.h"
#include "Shader.h"

#include "vulkan/vulkan.h"

namespace vk {
	// Singleton

	class VulkanRenderer : public Renderer {
	public:
		VulkanRenderer();
		virtual void draw() override;
		void destroy();

	public:
		VulkanContext m_context {};
		std::unique_ptr<Swapchain> m_swapchain;
		VkRenderPass m_renderpass {};
		VkDescriptorSetLayout m_descriptor_set_layout {};
		std::shared_ptr<Shader> m_shader;
		VkPipelineLayout m_pipeline_layout;
	};
}