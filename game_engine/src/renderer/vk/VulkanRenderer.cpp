#include "pch.h"
#include "VulkanRenderer.h"
#include "Init.h"
#include "ValidationLayers.h"

#include <vector>

#include "GLFW/glfw3.h"

#define VALIDATION_LAYERS_ENABLE true

namespace vk {

	VulkanRenderer::VulkanRenderer() {
		m_context.enable_validation_layers = VALIDATION_LAYERS_ENABLE;
		m_context.instance = init::createInstance(VALIDATION_LAYERS_ENABLE);
		setupDebugMessenger(m_context.instance, &m_context.debug_messenger, VALIDATION_LAYERS_ENABLE);

		g_window->create_vulkan_surface(m_context.instance, &m_context.surface);
		m_context.physical_device = init::createPhysicalDevice(
			m_context.instance, m_context.surface, &m_context.msaa_samples
		);
		m_context.device = init::createLogicalDevice(m_context.physical_device, m_context.surface, VALIDATION_LAYERS_ENABLE);
		m_context.command_pool = init::create_commmand_pool(m_context);
		m_descriptor_pool = init::create_descriptor_pool(m_context);

		QueueFamilyIndices indices = init::findQueueFamilies(m_context.physical_device, m_context.surface);
		vkGetDeviceQueue(m_context.device, indices.graphicsFamily.value(), 0, &m_context.graphics_queue);
		vkGetDeviceQueue(m_context.device, indices.presentFamily.value(), 0, &m_context.present_queue);

		m_renderpass = init::createRenderPass(m_context);
		m_swapchain = std::make_unique<Swapchain>(m_context, m_renderpass);
		m_descriptor_set_layout = init::createDescriptorSetLayout(m_context);
		m_shader = std::make_shared<Shader>(m_context, "resources/shaders/vert.spv", "resources/shaders/frag.spv");

		m_graphics_pipeline = init::createGraphicsPipeline(m_context, m_shader, m_swapchain, m_renderpass, m_descriptor_set_layout, &m_pipeline_layout);
		m_texture = std::make_shared<Texture>("resources/textures/viking_room.png", m_context);
		m_mesh = std::make_shared<Mesh>(m_context, "resources/models/viking_room.obj");
		for (int i = 0; i < m_context.MAX_FRAMES_IN_FLIGHT; i++) {
			m_uniform_buffers.emplace_back(std::make_shared<SharedBuffer>(m_context, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UniformBufferObject)));
		}
	}
	void VulkanRenderer::draw() {

	}
	void VulkanRenderer::destroy(){

	}
}
