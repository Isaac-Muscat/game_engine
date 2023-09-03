#pragma once
#include "pch.h"
#include "hid/Window.h"
#include "renderer/Renderer.h"
#include "Swapchain.h"
#include "Shader.h"
#include "Image.h"
#include "Texture.h"
#include "Mesh.h"
#include "Buffer.h"

#include "vulkan/vulkan.h"

namespace vk {
	// Singleton

	class VulkanRenderer : public Renderer {
	public:
		VulkanRenderer();
		virtual void draw() override;
		void destroy();

	public:
		uint32_t m_current_frame = 0;

		VulkanContext m_context {};
		std::unique_ptr<Swapchain> m_swapchain;
		VkRenderPass m_renderpass {};
		std::shared_ptr<Shader> m_shader;
		VkPipelineLayout m_pipeline_layout;
		VkPipeline m_graphics_pipeline;
		VkDescriptorSetLayout m_descriptor_set_layout {};
		VkDescriptorPool m_descriptor_pool;

		std::shared_ptr<Texture> m_texture;
		std::shared_ptr<Mesh> m_mesh;

		std::vector<VkDescriptorSet> m_descriptor_sets;
		std::vector<std::shared_ptr<SharedBuffer>> m_uniform_buffers;
		std::vector<VkCommandBuffer> m_command_buffers;

		std::vector<VkSemaphore> m_image_available_semaphores;
		std::vector<VkSemaphore> m_render_finished_semaphores;
		std::vector<VkFence> m_in_flight_fences;
	};
}