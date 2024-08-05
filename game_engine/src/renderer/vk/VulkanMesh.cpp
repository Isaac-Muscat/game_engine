#include "pch.h"
#include "VulkanMesh.h"
#include "VulkanRenderer.h"

#include "glm/common.hpp"
#include <glm/gtx/hash.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace std {
    template<> struct hash<vk::Vertex> {
        size_t operator()(vk::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.texture_coords) << 1);
        }
    };
}

namespace vk {
	VkVertexInputBindingDescription Vertex::GetBindingDescription() {
		VkVertexInputBindingDescription binding_description{};
		binding_description.binding = 0;
		binding_description.stride = sizeof(Vertex);
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return binding_description;
	}

	std::array<VkVertexInputAttributeDescription, 4> Vertex::GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attribute_descriptions{};
		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = offsetof(Vertex, pos);

		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(Vertex, color);

		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_descriptions[2].offset = offsetof(Vertex, texture_coords);

		attribute_descriptions[3].binding = 0;
		attribute_descriptions[3].location = 3;
		attribute_descriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[3].offset = offsetof(Vertex, normal);

		return attribute_descriptions;
	}

	VulkanMesh::VulkanMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) 
        : m_vertices(vertices), m_indices(indices) {
        m_vertex_buffer = std::make_shared<VulkanDeviceBuffer>(g_renderer->GetContext(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_vertices.data(), sizeof(m_vertices[0]) * m_vertices.size());
        m_index_buffer = std::make_shared<VulkanDeviceBuffer>(g_renderer->GetContext(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_indices.data(), sizeof(m_indices[0]) * m_indices.size());
    }

	VulkanMesh::VulkanMesh(std::string filepath) {
        LoadModel(filepath);
        m_vertex_buffer = std::make_shared<VulkanDeviceBuffer>(g_renderer->GetContext(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, m_vertices.data(), sizeof(m_vertices[0]) * m_vertices.size());
        m_index_buffer = std::make_shared<VulkanDeviceBuffer>(g_renderer->GetContext(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, m_indices.data(), sizeof(m_indices[0]) * m_indices.size());
	}

    void VulkanMesh::LoadModel(std::string filepath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> unique_vertices{};

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};
                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
                vertex.texture_coords = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };
                vertex.color = { 1.0f, 1.0f, 1.0f };
                if (unique_vertices.count(vertex) == 0) {
                    unique_vertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                    m_vertices.push_back(vertex);
                }

                m_indices.push_back(unique_vertices[vertex]);
            }
        }
    }
}
