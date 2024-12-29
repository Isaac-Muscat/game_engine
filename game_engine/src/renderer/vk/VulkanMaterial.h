#pragma once
#include "pch.h"
#include "VulkanTexture.h"
#include "VulkanShader.h"
#include "renderer/vk/VulkanBuffer.h"
#include "renderer/vk/VulkanTypes.h"

namespace vk {
    #define MAX_MATERIAL_DATA 10
    struct MaterialData {
        alignas(16) glm::vec3 albedo = glm::vec3(1, 1, 1);
        alignas(4) float roughness = 0.0f;
        alignas(4) float metallic = 0.0f;
        alignas(4) float ao = 0.1f;
        
        bool operator!= (const MaterialData& d) {
            return (
                albedo      != d.albedo &&
                roughness   != d.roughness &&
                metallic    != d.metallic &&
                ao          != d.ao
            );
        }
    };
	// TODO: Add builder functionality.
	class VulkanMaterial {
	public:
        RenderPipeline m_pipeline;
        ShaderStages m_shader_stages;
        VkDescriptorSet m_descriptor_set;
		std::shared_ptr<VulkanTexture> m_texture;
        // Hardcoded to 10 untextured material types
        std::array<MaterialData, MAX_MATERIAL_DATA> m_material_data = {};
        std::shared_ptr<VulkanSharedBuffer> m_material_buffer;

		VulkanMaterial(std::shared_ptr<VulkanShader> vertex_shader, std::shared_ptr<VulkanShader> fragment_shader, std::shared_ptr<VulkanTexture> texture, std::array<MaterialData, MAX_MATERIAL_DATA> material_data);
		VulkanMaterial(std::shared_ptr<VulkanShader> vertex_shader, std::shared_ptr<VulkanShader> fragment_shader, std::shared_ptr<VulkanTexture> texture, MaterialData material_data);
	};
}
