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
		m_texture = std::make_shared<VulkanTexture>("assets/textures/viking_room.png", m_context);
		m_mesh = std::make_shared<Mesh>(m_context, "assets/models/viking_room.obj");
		for (int i = 0; i < m_context.MAX_FRAMES_IN_FLIGHT; i++) {
			m_uniform_buffers.emplace_back(std::make_shared<VulkanSharedBuffer>(m_context, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UniformBufferObject)));
		}
		m_descriptor_sets = init::CreateDescriptorSets(m_context, m_descriptor_set_layout, m_descriptor_pool, m_uniform_buffers, m_texture);
		m_command_buffers = init::CreateCommandBuffers(m_context);
		init::CreateSyncObjects(m_context, m_image_available_semaphores, m_render_finished_semaphores, m_in_flight_fences);
	}
	void VulkanRenderer::Draw() {
        vkWaitForFences(m_context.device, 1, &m_in_flight_fences[m_current_frame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(m_context.device, m_swapchain->m_swapchain, UINT64_MAX, m_image_available_semaphores[m_current_frame], VK_NULL_HANDLE, &imageIndex);

       /* if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }*/

        vkResetFences(m_context.device, 1, &m_in_flight_fences[m_current_frame]);

        // Update Uniforms
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain->m_extent.width / (float)m_swapchain->m_extent.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;
        m_uniform_buffers[m_current_frame]->UpdateData(m_context, &ubo, sizeof(UniformBufferObject));

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
        renderPassInfo.framebuffer = m_swapchain->m_framebuffers[imageIndex];
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

        VkBuffer vertexBuffers[] = { m_mesh->m_vertex_buffer->m_buffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_command_buffers[m_current_frame], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(m_command_buffers[m_current_frame], m_mesh->m_index_buffer->m_buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 0, 1, &m_descriptor_sets[m_current_frame], 0, nullptr);
        vkCmdDrawIndexed(m_command_buffers[m_current_frame], static_cast<uint32_t>(m_mesh->m_indices.size()), 1, 0, 0, 0);

        // End the renderpass
        vkCmdEndRenderPass(m_command_buffers[m_current_frame]);

        // End the command buffer
        if (vkEndCommandBuffer(m_command_buffers[m_current_frame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        std::array<VkCommandBuffer, 1> submitCommandBuffers =
        { m_command_buffers[m_current_frame] };

        // Synchronization and submission of command buffer to graphics queue
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { m_image_available_semaphores[m_current_frame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
        submitInfo.pCommandBuffers = submitCommandBuffers.data();

        VkSemaphore signalSemaphores[] = { m_render_finished_semaphores[m_current_frame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_context.graphics_queue, 1, &submitInfo, m_in_flight_fences[m_current_frame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        // Signal for an image to be presented from the present queue
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = { m_swapchain->m_swapchain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(m_context.present_queue, &presentInfo);

        //if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        //    framebufferResized = false;
        //    recreateSwapChain();
        //}
        //else if (result != VK_SUCCESS) {
        //    throw std::runtime_error("failed to present swap chain image!");
        //}

        m_current_frame = (m_current_frame+ 1) % m_context.MAX_FRAMES_IN_FLIGHT;
	}
	void VulkanRenderer::Destroy(){

	}
}
