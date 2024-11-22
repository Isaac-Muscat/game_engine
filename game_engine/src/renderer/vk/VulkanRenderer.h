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

#include "renderer/vk/VulkanTypes.h"
#include "vulkan/vulkan.h"

namespace vk {
	class VulkanRenderer {
	public:
		VulkanRenderer();
		const VulkanContext& GetContext() const { return m_context; }
		void BeginFrame(const Camera& camera, std::vector<Light>& lights);
		void Draw(const std::shared_ptr<VulkanMesh>& mesh, glm::mat4 transform, const std::shared_ptr<VulkanMaterial>& material);
		void EndFrame();
		void GetNextBuffer();
        
        // Have a different pipeline depending on material...?
        RenderPipeline LoadDefaultPipeline(ShaderStages stages);
	public:
		VulkanContext m_context {};
		VkRenderPass m_renderpass {};
		std::unique_ptr<VulkanSwapchain> m_swapchain;
        std::unordered_map<std::string, RenderPipeline> m_pipeline_cache;

        std::vector<VkDescriptorSetLayout> m_descriptor_set_layouts;
		VkDescriptorPool m_descriptor_pool;
		VkDescriptorPool m_scene_descriptor_pool;
        std::vector<VkDescriptorSet> m_scene_descriptor_sets;
		std::vector<std::shared_ptr<VulkanSharedBuffer>> m_uniform_buffers;
		std::vector<std::shared_ptr<VulkanSharedBuffer>> m_scene_buffers;

		std::vector<VkCommandBuffer> m_command_buffers;
		std::vector<VkCommandBuffer> m_submissions = {};

		Camera m_current_camera;
		std::vector<Light>* m_current_lights;

        uint32_t m_num_lights = 1;
		uint32_t m_current_frame = 0;

        VkPipeline m_last_pipeline = VK_NULL_HANDLE;
        std::shared_ptr<VulkanMaterial> m_last_material = VK_NULL_HANDLE;
	};
}

extern std::unique_ptr<vk::VulkanRenderer> g_renderer;
