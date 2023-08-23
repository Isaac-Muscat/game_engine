#include "renderer/vk/Renderer.h"
#include "renderer/vk/Init.h"
#include "renderer/vk/ValidationLayers.h"
#include "renderer/vk/Shader.h"

#include <vector>

#include "GLFW/glfw3.h"

#define VALIDATION_LAYERS_ENABLE true

namespace vk {
	Renderer gRenderer;

	void Renderer::init(const Window& window) {
		m_context.enable_validation_layers = VALIDATION_LAYERS_ENABLE;
		m_context.instance = init::createInstance(VALIDATION_LAYERS_ENABLE, window);
		setupDebugMessenger(m_context.instance, &m_context.debug_messenger, VALIDATION_LAYERS_ENABLE);

		window.CreateSurface(m_context.instance, &m_context.surface);
		m_context.physical_device = init::createPhysicalDevice(
			m_context.instance, m_context.surface, &m_context.msaa_samples
		);
		m_context.device = init::createLogicalDevice(m_context.physical_device, m_context.surface, VALIDATION_LAYERS_ENABLE);

		QueueFamilyIndices indices = init::findQueueFamilies(m_context.physical_device, m_context.surface);
		vkGetDeviceQueue(m_context.device, indices.graphicsFamily.value(), 0, &m_context.graphics_queue);
		vkGetDeviceQueue(m_context.device, indices.presentFamily.value(), 0, &m_context.present_queue);

		m_swapchain = std::make_unique<Swapchain>(m_context, window);
		m_renderpass = init::createRenderPass(m_context, m_swapchain);
		m_descriptor_set_layout = init::createDescriptorSetLayout(m_context);
		m_shader = std::make_shared<Shader>(m_context, "resources/shaders/vert.spv", "resources/shaders/frag.spv");

		//m_graphics_pipeline = init::createGraphicsPipeline();
	}
	void Renderer::draw() {

	}
	void Renderer::destroy(){
	}
}
