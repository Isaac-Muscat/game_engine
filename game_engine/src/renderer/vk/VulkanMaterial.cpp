#include "pch.h"
#include "VulkanMaterial.h"
#include "VulkanRenderer.h"
#include "VulkanInit.h"
#include "renderer/vk/VulkanBuffer.h"

vk::VulkanMaterial::VulkanMaterial(std::shared_ptr<VulkanShader> vertex_shader, std::shared_ptr<VulkanShader> fragment_shader, std::shared_ptr<VulkanTexture> texture) : m_texture(texture), m_shader_stages({vertex_shader, fragment_shader}) {
    m_material_data = std::make_shared<VulkanSharedBuffer>(g_renderer->GetContext(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(MaterialData));
    MaterialData data{};
    data.albedo = glm::vec3(1.0f, 0.0f, 0.0f);
    data.metallic = 0.1f;
    data.roughness = 0.5f;
    data.ao = 0.03f;
    m_material_data->UpdateData(g_renderer->GetContext(), &data, sizeof(MaterialData));
    m_pipeline = g_renderer->LoadDefaultPipeline(m_shader_stages);
	m_descriptor_set = init::CreateDescriptorSets(g_renderer->GetContext(), g_renderer->m_descriptor_set_layouts[1], g_renderer->m_descriptor_pool, m_texture, m_material_data);
}
