#pragma once
#include "pch.h"
#include "Buffer.h"
#include "glm/glm.hpp"
#include "vulkan/vulkan.h"


namespace vk {
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
    };

	class Mesh {
    public:
        std::vector<Vertex> m_vertices;
        std::vector<uint32_t> m_indices;
        std::shared_ptr<DeviceBuffer> m_vertex_buffer;
        std::shared_ptr<DeviceBuffer> m_index_buffer;
        Mesh(const VulkanContext& context, std::string filepath);
        void load_model(std::string filepath);
	};
}
