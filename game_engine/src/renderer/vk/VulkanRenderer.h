#pragma once
#include "pch.h"
#include "hid/Window.h"
#include "renderer/Renderer.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanImage.h"
#include "VulkanTexture.h"
#include "VulkanMesh.h"
#include "VulkanBuffer.h"

#include "vulkan/vulkan.h"

namespace vk {
	class VulkanRenderer : public Renderer {
	public:
		VulkanRenderer();
		void Draw() override;
		void SwapBuffers() override;
	public:
		uint32_t m_current_frame = 0;
		VulkanContext m_context {};
		std::unique_ptr<VulkanSwapchain> m_swapchain;
		VkRenderPass m_renderpass {};
		std::shared_ptr<VulkanShader> m_shader;
		VkPipelineLayout m_pipeline_layout;
		VkPipeline m_graphics_pipeline;
		VkDescriptorSetLayout m_descriptor_set_layout {};
		VkDescriptorPool m_descriptor_pool;
		std::shared_ptr<VulkanTexture> m_texture;
		std::shared_ptr<Mesh> m_mesh;
		std::vector<VkDescriptorSet> m_descriptor_sets;
		std::vector<std::shared_ptr<VulkanSharedBuffer>> m_uniform_buffers;
		std::vector<VkCommandBuffer> m_command_buffers;
		std::vector<VkSemaphore> m_image_available_semaphores;
		std::vector<VkSemaphore> m_render_finished_semaphores;
		std::vector<VkFence> m_in_flight_fences;
	};
}