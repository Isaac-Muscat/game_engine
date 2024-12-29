#pragma once
#include "pch.h"
#include "VulkanBuffer.h"

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"


namespace vk {
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texture_coords;
        glm::vec3 normal;
        uint8_t material_id = 0;
        

        static VkVertexInputBindingDescription GetBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 5> GetAttributeDescriptions();
        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && texture_coords == other.texture_coords && normal == other.normal;
        }
    };

	class VulkanMesh {
    public:
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
        std::shared_ptr<VulkanDeviceBuffer> m_vertex_buffer;
        std::shared_ptr<VulkanDeviceBuffer> m_index_buffer;
        VulkanMesh(std::string filepath);
        VulkanMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
        void LoadModel(std::string filepath);
	};
}
