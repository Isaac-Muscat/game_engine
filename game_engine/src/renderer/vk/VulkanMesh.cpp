#include "pch.h"
#include "VulkanMesh.h"
#include "VulkanRenderer.h"

#include "glm/common.hpp"
#include <glm/gtx/hash.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "tiny_gltf.h"

//TODO Need some sort of builder to easily make vertex object data easier to build
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

std::array<VkVertexInputAttributeDescription, 5> Vertex::GetAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 5> attribute_descriptions{};
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

    attribute_descriptions[4].binding = 0;
    attribute_descriptions[4].location = 4;
    attribute_descriptions[4].format = VK_FORMAT_R8_UINT;
    attribute_descriptions[4].offset = offsetof(Vertex, material_id);

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
    std::string file_extension = "";
    if (filepath.find_last_of(".") != std::string::npos) {
        file_extension = filepath.substr(filepath.find_last_of(".") + 1);
    }

    if (file_extension == "obj") {
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
                vertex.material_id = 0;
                m_indices.push_back(unique_vertices[vertex]);
            }
        }
    } else if (file_extension == "glb") {
        tinygltf::Model model;
        tinygltf::TinyGLTF gltf_ctx;
        std::string err;
        std::string warn;
        bool ret = false;

        ret = gltf_ctx.LoadBinaryFromFile(&model, &err, &warn, filepath.c_str());
        if (!warn.empty()) {
            printf("Warn: %s\n", warn.c_str());
        }
        if (!err.empty()) {
            printf("Err: %s\n", err.c_str());
        }
        if (!ret) {
            printf("Failed to parse glTF\n");
        }

        auto mesh = model.meshes[0];
        uint32_t global_index_offset = 0;
        for (auto prim : mesh.primitives) {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            auto accessor = model.accessors[prim.indices];
            auto buffer_view = model.bufferViews[accessor.bufferView];
            auto buffer = model.buffers[buffer_view.buffer];
            uint32_t max_index = 0;
            for (int i = buffer_view.byteOffset; i < buffer_view.byteOffset + buffer_view.byteLength; i+=sizeof(uint16_t)) {
                uint16_t index;
                memcpy(&index, buffer.data.data()+i, sizeof(uint16_t));
                uint32_t int_index = index + global_index_offset;
                max_index = std::max(max_index, int_index);
                indices.push_back(int_index);
            }
            global_index_offset = max_index + 1;

            accessor = model.accessors[prim.attributes["POSITION"]];
            buffer_view = model.bufferViews[accessor.bufferView];
            buffer = model.buffers[buffer_view.buffer];
            for (int i = buffer_view.byteOffset; i < buffer_view.byteOffset + buffer_view.byteLength; i+=sizeof(glm::vec3)) {
                float posx, posy, posz;
                memcpy(&posx, buffer.data.data() + i + sizeof(float) * 0, sizeof(float));
                memcpy(&posy, buffer.data.data() + i + sizeof(float) * 1, sizeof(float));
                memcpy(&posz, buffer.data.data() + i + sizeof(float) * 2, sizeof(float));
                glm::vec3 pos(posx, posy, posz);

                vertices.push_back({pos, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0), glm::vec3(1), (uint8_t)prim.material});
            }

            // TODO Make loading a gltf its own thing in asset manager so mesh is loaded with material(s) created in asset manager
            // Then a list of entities are return
            auto n_accessor = model.accessors[prim.attributes["NORMAL"]];
            auto n_buffer_view = model.bufferViews[n_accessor.bufferView];
            auto n_buffer = model.buffers[n_buffer_view.buffer];
            for (int i = n_buffer_view.byteOffset; i < n_buffer_view.byteOffset + n_buffer_view.byteLength; i+=sizeof(glm::vec3)) {
                float nx, ny, nz;
                memcpy(&nx, n_buffer.data.data() + i + sizeof(float) * 0, sizeof(float));
                memcpy(&ny, n_buffer.data.data() + i + sizeof(float) * 1, sizeof(float));
                memcpy(&nz, n_buffer.data.data() + i + sizeof(float) * 2, sizeof(float));
                glm::vec3 normal(nx, ny, nz);
                int index = floor((i - n_buffer_view.byteOffset)/sizeof(glm::vec3));
                vertices[index].normal = normal;
            }
            auto t_accessor = model.accessors[prim.attributes["TEXCOORD_0"]];
            auto t_buffer_view = model.bufferViews[t_accessor.bufferView];
            auto t_buffer = model.buffers[t_buffer_view.buffer];
            for (int i = t_buffer_view.byteOffset; i < t_buffer_view.byteOffset + t_buffer_view.byteLength; i+=sizeof(glm::vec2)) {
                float tx, ty;
                memcpy(&tx, t_buffer.data.data() + i + sizeof(float) * 0, sizeof(float));
                memcpy(&ty, t_buffer.data.data() + i + sizeof(float) * 1, sizeof(float));
                glm::vec2 uv(tx, ty);
                int index = floor((i - t_buffer_view.byteOffset)/sizeof(glm::vec2));
                vertices[index].texture_coords = uv;
            }
            m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
            m_indices.insert(m_indices.end(), indices.begin(), indices.end());
        }
    }
}

}
