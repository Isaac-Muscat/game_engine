#include "pch.h"
#include "VulkanRenderer.h"
#include "VulkanInit.h"
#include "VulkanValidationLayers.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GLFW/glfw3.h"

#define VALIDATION_LAYERS_ENABLE true

namespace vk {
	VulkanRenderer::VulkanRenderer() {
		m_context.enable_validation_layers = VALIDATION_LAYERS_ENABLE;
		m_context.instance = init::CreateInstance(VALIDATION_LAYERS_ENABLE);
		SetupDebugMessenger(m_context.instance, &m_context.debug_messenger, VALIDATION_LAYERS_ENABLE);

		g_window->CreateVulkanSurface(m_context.instance, &m_context.surface);
		m_context.physical_device = init::CreatePhysicalDevice(
			m_context.instance, m_context.surface, &m_context.msaa_samples
		);
		m_context.device = init::CreateLogicalDevice(m_context.physical_device, m_context.surface, VALIDATION_LAYERS_ENABLE);
		m_context.command_pool = init::CreateCommandPool(m_context);
		m_descriptor_pool = init::CreateDescriptorPool(m_context);

		QueueFamilyIndices indices = init::FindQueueFamilies(m_context.physical_device, m_context.surface);
		vkGetDeviceQueue(m_context.device, indices.graphics_family.value(), 0, &m_context.graphics_queue);
		vkGetDeviceQueue(m_context.device, indices.present_family.value(), 0, &m_context.present_queue);

		m_renderpass = init::CreateRenderPass(m_context);
		m_swapchain = std::make_unique<VulkanSwapchain>(m_context, m_renderpass);
		m_descriptor_set_layout = init::CreateDescriptorSetLayout(m_context);
		m_shader = std::make_shared<VulkanShader>(m_context, "assets/shaders/vert.spv", "assets/shaders/frag.spv");

		m_graphics_pipeline = init::CreateGraphicsPipeline(m_context, m_shader, m_swapchain, m_renderpass, m_descriptor_set_layout, &m_pipeline_layout);
		for (int i = 0; i < m_context.MAX_FRAMES_IN_FLIGHT; i++) {
			m_uniform_buffers.emplace_back(std::make_shared<VulkanSharedBuffer>(m_context, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UniformBufferObject)));
		}
		m_command_buffers = init::CreateCommandBuffers(m_context);
	}

    void VulkanRenderer::BeginFrame(const Camera& camera) {
        m_current_camera = camera;

        // Reset/Clear commands of buffer
        vkResetCommandBuffer(m_command_buffers[m_current_frame], 0);

        // Start a command buffer
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(m_command_buffers[m_current_frame], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        // Start a renderpass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderpass;
        renderPassInfo.framebuffer = m_swapchain->GetCurrentFramebuffer();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_swapchain->m_extent;
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_command_buffers[m_current_frame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind the pipline
        vkCmdBindPipeline(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);

        // Populate the command buffer
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_swapchain->m_extent.width);
        viewport.height = static_cast<float>(m_swapchain->m_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_command_buffers[m_current_frame], 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_swapchain->m_extent;
        vkCmdSetScissor(m_command_buffers[m_current_frame], 0, 1, &scissor);
    }

    void VulkanRenderer::EndFrame() {
        // End the renderpass
        vkCmdEndRenderPass(m_command_buffers[m_current_frame]);

        // End the command buffer
        if (vkEndCommandBuffer(m_command_buffers[m_current_frame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        m_submissions.emplace_back(m_command_buffers[m_current_frame]);
    }

	void VulkanRenderer::Draw(const std::shared_ptr<VulkanMesh>& mesh, glm::mat4 transform, const std::shared_ptr<VulkanMaterial>& material) {
        // Update Uniforms
        UniformBufferObject ubo{};
        //ubo.model = transform;
        ubo.view = m_current_camera.GetViewMatrix();
        ubo.proj = m_current_camera.GetProjectionMatrix(m_swapchain->m_extent.width / (float)m_swapchain->m_extent.height);
        ubo.proj[1][1] *= -1;
        m_uniform_buffers[m_current_frame]->UpdateData(m_context, &ubo, sizeof(UniformBufferObject));

        ModelPushConstant model = { transform };

        VkBuffer vertexBuffers[] = { mesh->m_vertex_buffer->m_buffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_command_buffers[m_current_frame], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(m_command_buffers[m_current_frame], mesh->m_index_buffer->m_buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, material->GetDescriptorSet(m_current_frame), 0, nullptr);
        vkCmdPushConstants(m_command_buffers[m_current_frame], m_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelPushConstant), &model);
        vkCmdDrawIndexed(m_command_buffers[m_current_frame], static_cast<uint32_t>(mesh->m_indices.size()), 1, 0, 0, 0);
	}

    void VulkanRenderer::GetNextBuffer() {
        if (m_submissions.size() > 0) {
            m_swapchain->Submit(m_submissions, m_current_frame);
            m_submissions.clear();
            m_current_frame = (m_current_frame + 1) % m_context.MAX_FRAMES_IN_FLIGHT;
        }
        m_swapchain->GetNextImage(m_current_frame);
    }
}
