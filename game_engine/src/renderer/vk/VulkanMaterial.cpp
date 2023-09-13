#include "pch.h"
#include "VulkanMaterial.h"
#include "VulkanRenderer.h"
#include "VulkanInit.h"

vk::VulkanMaterial::VulkanMaterial(std::shared_ptr<VulkanTexture> texture) : m_texture(texture) {
	m_descriptor_sets = init::CreateDescriptorSets(g_renderer->GetContext(), g_renderer->m_descriptor_set_layout, g_renderer->m_descriptor_pool, g_renderer->m_uniform_buffers, m_texture);
}
