#pragma once
#include "pch.h"
#include "hid/Window.h"
#include "renderer/Renderer.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanImage.h"
#include "VulkanMaterial.h"
#include "VulkanMesh.h"
#include "VulkanBuffer.h"
#include "renderer/Camera.h"
#include "ecs/Components.h"
#include "ecs/ComponentArray.h"

#include "vulkan/vulkan.h"

namespace vk {
	class VulkanRenderer {
	public:
		VulkanRenderer();
		const VulkanContext& GetContext() const { return m_context; }
		void BeginFrame(const Camera& camera, const ComponentArray<LightComponent>& lights);
		// TODO: Change texture to material.
		void Draw(const std::shared_ptr<VulkanMesh>& mesh, glm::mat4 transform, const std::shared_ptr<VulkanMaterial>& material);
		void EndFrame();
		void GetNextBuffer();
	public:
		Camera m_current_camera;

		VulkanContext m_context {};
		VkRenderPass m_renderpass {};
		std::shared_ptr<VulkanShader> m_shader;
		VkPipeline m_graphics_pipeline;
		VkPipelineLayout m_pipeline_layout;
        std::vector<VkDescriptorSetLayout> m_descriptor_set_layouts;
		VkDescriptorPool m_descriptor_pool;
		VkDescriptorPool m_light_descriptor_pool;
		
		std::vector<std::shared_ptr<VulkanSharedBuffer>> m_uniform_buffers;
		std::vector<std::shared_ptr<VulkanSharedBuffer>> m_scene_buffers;
        std::vector<VkDescriptorSet> m_lights_descriptor_sets;
        int m_num_lights = 1;
		std::vector<VkCommandBuffer> m_command_buffers;
		std::vector<VkCommandBuffer> m_submissions = {};

		std::unique_ptr<VulkanSwapchain> m_swapchain;
		uint32_t m_current_frame = 0;
	};
}

extern std::unique_ptr<vk::VulkanRenderer> g_renderer;
