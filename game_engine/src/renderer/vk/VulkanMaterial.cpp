#include "pch.h"
#include "VulkanMaterial.h"
#include "VulkanRenderer.h"
#include "VulkanInit.h"
#include "renderer/vk/VulkanBuffer.h"

vk::VulkanMaterial::VulkanMaterial(std::shared_ptr<VulkanShader> vertex_shader, std::shared_ptr<VulkanShader> fragment_shader, std::shared_ptr<VulkanTexture> texture, MaterialData material_data) : m_texture(texture), m_shader_stages({vertex_shader, fragment_shader}) {
    std::array<MaterialData, MAX_MATERIAL_DATA> material_data_array;
    material_data_array[0] = material_data;
    m_material_buffer = std::make_shared<VulkanSharedBuffer>(g_renderer->GetContext(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, MAX_MATERIAL_DATA * sizeof(MaterialData));
    m_material_buffer->UpdateData(g_renderer->GetContext(), &material_data_array, MAX_MATERIAL_DATA * sizeof(MaterialData));
    m_pipeline = g_renderer->LoadDefaultPipeline(m_shader_stages);
    m_descriptor_set = init::CreateDescriptorSets(g_renderer->GetContext(), g_renderer->m_descriptor_set_layouts[1], g_renderer->m_descriptor_pool, m_texture, m_material_buffer);
}
vk::VulkanMaterial::VulkanMaterial(std::shared_ptr<VulkanShader> vertex_shader, std::shared_ptr<VulkanShader> fragment_shader, std::shared_ptr<VulkanTexture> texture, std::array<MaterialData, MAX_MATERIAL_DATA> material_data) : m_texture(texture), m_shader_stages({vertex_shader, fragment_shader}), m_material_data(material_data) {
    m_material_buffer = std::make_shared<VulkanSharedBuffer>(g_renderer->GetContext(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, MAX_MATERIAL_DATA * sizeof(MaterialData));
    m_material_buffer->UpdateData(g_renderer->GetContext(), &m_material_data, MAX_MATERIAL_DATA * sizeof(MaterialData));
    m_pipeline = g_renderer->LoadDefaultPipeline(m_shader_stages);
	m_descriptor_set = init::CreateDescriptorSets(g_renderer->GetContext(), g_renderer->m_descriptor_set_layouts[1], g_renderer->m_descriptor_pool, m_texture, m_material_buffer);
}
