#include "glm/ext/matrix_transform.hpp"
#include "pch.h"
#include "ecs/ComponentArray.h"
#include "ecs/Components.h"
#include "core/AssetManager.h"
#include "VulkanRenderer.h"
#include "VulkanInit.h"
#include "VulkanValidationLayers.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GLFW/glfw3.h"
#include "renderer/Light.h"
#include "vulkan/vulkan_core.h"

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

        QueueFamilyIndices indices = init::FindQueueFamilies(m_context.physical_device, m_context.surface);
        vkGetDeviceQueue(m_context.device, indices.graphics_family.value(), 0, &m_context.graphics_queue);
        vkGetDeviceQueue(m_context.device, indices.present_family.value(), 0, &m_context.present_queue);

        // TODO: Need some sort of descriptor allocator
        m_descriptor_pool = init::CreateDescriptorPool(m_context);
        m_scene_descriptor_pool = init::CreateSceneDescriptorPool(m_context);

		m_renderpass = init::CreateRenderPass(m_context);
		m_swapchain = std::make_unique<VulkanSwapchain>(m_context, m_renderpass);

		m_descriptor_set_layouts = init::CreateDescriptorSetLayouts(m_context);

		for (int i = 0; i < m_context.MAX_FRAMES_IN_FLIGHT; i++) {
			m_uniform_buffers.emplace_back(std::make_shared<VulkanSharedBuffer>(m_context, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UniformBufferObject)));
			m_scene_buffers.emplace_back(std::make_shared<VulkanSharedBuffer>(m_context, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(Light) * m_num_lights));
		}

        m_scene_descriptor_sets = init::CreateSceneDescriptorSets(m_context, m_descriptor_set_layouts[0], m_scene_descriptor_pool, m_scene_buffers, m_uniform_buffers, m_num_lights);
		m_command_buffers = init::CreateCommandBuffers(m_context);
	}

    void VulkanRenderer::BeginFrame(const Camera& camera, std::vector<Light>& lights) {
        std::cout << "Begin frame" << "\n";
        m_current_camera = camera;
        m_current_lights = &lights;

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
        clearValues[0].color = { {0.5f, 0.8f, 1.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_command_buffers[m_current_frame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        m_last_pipeline = VK_NULL_HANDLE;
        m_last_material = nullptr;
    }

	void VulkanRenderer::Draw(const std::shared_ptr<VulkanMesh>& mesh, glm::mat4 transform, const std::shared_ptr<VulkanMaterial>& material) {
        if (m_last_material != material) {
            //std::cout << "Material changed" << "\n";
            m_last_material = material;
            if (m_last_pipeline != material->m_pipeline.pipeline) {
                //std::cout << "Pipeline changed" << "\n";
                m_last_pipeline = material->m_pipeline.pipeline;
                // Bind the pipline
                vkCmdBindPipeline(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, material->m_pipeline.pipeline);

                // Bind the lights
                if (m_current_lights->size() != m_num_lights) {
                    m_num_lights = m_current_lights->size();
                    vkDeviceWaitIdle(m_context.device); // TODO: DONT CALL vkDeviceWaitIdle BUT MAINTAIN SYNCHRONIZATION
                    m_scene_buffers.clear();
                    for (int i = 0; i < m_context.MAX_FRAMES_IN_FLIGHT; i++) {

                        m_scene_buffers.emplace_back(std::make_shared<VulkanSharedBuffer>(m_context, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(Light) * m_num_lights));
                    }
                    init::LightsUpdateDescriptorSets(m_context, m_descriptor_set_layouts[1], m_descriptor_pool, m_scene_buffers , m_num_lights, m_scene_descriptor_sets);
                }
                m_scene_buffers[m_current_frame]->UpdateData(m_context, m_current_lights->data(), sizeof(Light) * m_current_lights->size());

                // Update Camera Uniforms
                UniformBufferObject ubo{};
                ubo.view = m_current_camera.GetViewMatrix();
                ubo.proj = m_current_camera.GetProjectionMatrix(m_swapchain->m_extent.width / (float)m_swapchain->m_extent.height);
                ubo.proj[1][1] *= -1;
                m_uniform_buffers[m_current_frame]->UpdateData(m_context, &ubo, sizeof(UniformBufferObject));

                vkCmdBindDescriptorSets(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, material->m_pipeline.pipeline_layout, 0, 1, &m_scene_descriptor_sets[m_current_frame], 0, nullptr);

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
            vkCmdBindDescriptorSets(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, material->m_pipeline.pipeline_layout, 1, 1, &material->m_descriptor_set, 0, nullptr);
        }

        VkBuffer vertexBuffers[] = { mesh->m_vertex_buffer->m_buffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_command_buffers[m_current_frame], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(m_command_buffers[m_current_frame], mesh->m_index_buffer->m_buffer, 0, VK_INDEX_TYPE_UINT32);

        ModelPushConstant model = { transform };
        vkCmdPushConstants(m_command_buffers[m_current_frame], material->m_pipeline.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelPushConstant), &model);
        vkCmdDrawIndexed(m_command_buffers[m_current_frame], static_cast<uint32_t>(mesh->m_indices.size()), 1, 0, 0, 0);
	}

    void VulkanRenderer::DrawGUI() {
        
    }


    void VulkanRenderer::EndFrame() {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        vk::Vertex v1, v2, v3, v4;

        glm::vec3 color(1.0f);
        uint32_t index = 0;

        v1 = { glm::vec3(0.0f, -0.5f, 0.5f), color, glm::vec2(0), glm::vec3(0, 1, 0) };
        v2 = { glm::vec3(0.0f, -0.5f, -0.5f), color, glm::vec2(0), glm::vec3(0, 1, 0) };
        v3 = { glm::vec3(0.0f, 0.5f, -0.5f), color, glm::vec2(0), glm::vec3(0, 1, 0) };
        v4 = { glm::vec3(0.0f, 0.5f, 0.5f), color, glm::vec2(0), glm::vec3(0, 1, 0) };
        v1.normal = glm::vec3(0, 1, 0);
        v2.normal = glm::vec3(0, 1, 0);
        v3.normal = glm::vec3(0, 1, 0);
        v4.normal = glm::vec3(0, 1, 0);
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);
        indices.push_back(index);
        indices.push_back(index + 2);
        indices.push_back(index + 1);
    
        indices.push_back(index);
        indices.push_back(index + 3);
        indices.push_back(index + 2);

        std::shared_ptr<VulkanMesh> mesh = std::make_shared<VulkanMesh>(vertices, indices);
        glm::mat4 transform = glm::identity<glm::mat4>();

        auto material = Assets::LoadMaterial("assets/materials/default_white.mat");
        vkCmdBindPipeline(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, material->m_pipeline.pipeline);
        vkCmdBindDescriptorSets(m_command_buffers[m_current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, material->m_pipeline.pipeline_layout, 1, 1, &material->m_descriptor_set, 0, nullptr);
        VkBuffer vertexBuffers[] = { mesh->m_vertex_buffer->m_buffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_command_buffers[m_current_frame], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(m_command_buffers[m_current_frame], mesh->m_index_buffer->m_buffer, 0, VK_INDEX_TYPE_UINT32);

        ModelPushConstant model = { transform };
        vkCmdPushConstants(m_command_buffers[m_current_frame], material->m_pipeline.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelPushConstant), &model);
        vkCmdDrawIndexed(m_command_buffers[m_current_frame], static_cast<uint32_t>(mesh->m_indices.size()), 1, 0, 0, 0);

        // End the renderpass
        vkCmdEndRenderPass(m_command_buffers[m_current_frame]);

        // End the command buffer
        if (vkEndCommandBuffer(m_command_buffers[m_current_frame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        m_submissions.emplace_back(m_command_buffers[m_current_frame]);
    }

    void VulkanRenderer::GetNextBuffer() {
        if (m_submissions.size() > 0) {
            m_swapchain->Submit(m_submissions, m_current_frame);
            m_submissions.clear();
            m_current_frame = (m_current_frame + 1) % m_context.MAX_FRAMES_IN_FLIGHT;
        }
        m_swapchain->GetNextImage(m_current_frame);
    }
    
    RenderPipeline VulkanRenderer::LoadDefaultPipeline(ShaderStages stages) {
        std::string key = stages.vertex_shader->m_filepath + stages.fragment_shader->m_filepath;
        if(m_pipeline_cache.find(key) == m_pipeline_cache.end()) {
            VkPipelineLayout pipeline_layout;
            VkPipeline pipeline = init::CreateGraphicsPipeline(m_context, stages.vertex_shader, stages.fragment_shader, m_swapchain, m_renderpass, m_descriptor_set_layouts, &pipeline_layout);
            m_pipeline_cache[key] = {pipeline, pipeline_layout};
        }
        return m_pipeline_cache[key];
    }
}
