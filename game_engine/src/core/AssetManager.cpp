#include "pch.h"
#include "renderer/vk/VulkanMaterial.h"
#include "renderer/vk/VulkanRenderer.h"
#include "AssetManager.h"
#include "tiny_gltf.h"

namespace Assets {
    std::shared_ptr<vk::VulkanMesh> LoadMesh(std::string path) { return g_asset_manager->LoadMesh(path); }
    std::shared_ptr<vk::VulkanTexture> LoadTexture(std::string path) { return g_asset_manager->LoadTexture(path); }
    std::shared_ptr<vk::VulkanMaterial> LoadMaterial(std::string path) { return g_asset_manager->LoadMaterial(path); }
    std::shared_ptr<vk::VulkanShader> LoadShader(std::string path) { return g_asset_manager->LoadShader(path); }
}

std::shared_ptr<vk::VulkanMesh> AssetManager::LoadMesh(std::string path) {
    if(m_meshes.find(path) == m_meshes.end()) {
        m_meshes[path] = std::make_shared<vk::VulkanMesh>(path);
    }
    return m_meshes[path];
}

std::shared_ptr<vk::VulkanTexture> AssetManager::LoadTexture(std::string path) {
    if(m_textures.find(path) == m_textures.end()) {
        m_textures[path] = std::make_shared<vk::VulkanTexture>(path);
    }
    return m_textures[path];
}

std::shared_ptr<vk::VulkanShader> AssetManager::LoadShader(std::string path) {
    if(m_shaders.find(path) == m_shaders.end()) {
        m_shaders[path] = std::make_shared<vk::VulkanShader>(g_renderer->GetContext(), path);
    }
    return m_shaders[path];
}

std::shared_ptr<vk::VulkanMaterial> AssetManager::LoadMaterial(std::string path) {
    if(m_materials.find(path) == m_materials.end()) {
        std::string file_extension = "";
        if (path.find_last_of(".") != std::string::npos) {
            file_extension = path.substr(path.find_last_of(".") + 1);
        }

        if (file_extension == "mat") {
            std::ifstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("failed to open file!");
            }

            std::shared_ptr<vk::VulkanTexture> texture;
            std::shared_ptr<vk::VulkanShader> vertex_shader;
            std::shared_ptr<vk::VulkanShader> fragment_shader;
            vk::MaterialData material_data;

            std::string line;
            while (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string key;
                std::string value;
                std::getline(ss, key, ' ');
                std::getline(ss, value, ' ');
                if (key == "texture") {
                    texture = LoadTexture(value);
                } else if (key == "vertex_shader") {
                    vertex_shader = LoadShader(value);
                } else if (key == "fragment_shader") {
                    fragment_shader = LoadShader(value);
                } else if (key == "albedo") {
                    std::string sg, sb;
                    std::getline(ss, sg, ' ');
                    std::getline(ss, sb, ' ');
                    material_data.albedo = glm::vec3(atof(value.data()), atof(sg.data()), atof(sb.data()));
                } else if(key == "roughness") {
                    material_data.roughness = atof(value.data());
                } else if(key == "metallic") {
                    material_data.metallic = atof(value.data());
                } else if(key == "ao") {
                    material_data.ao = atof(value.data());
                }
            }
            m_materials[path] = std::make_shared<vk::VulkanMaterial>(vertex_shader, fragment_shader, texture, material_data);
        } else if (file_extension == "glb") {
            std::shared_ptr<vk::VulkanShader> vertex_shader = LoadShader("assets/shaders/pbr_vert.spv");
            std::shared_ptr<vk::VulkanShader> fragment_shader = LoadShader("assets/shaders/pbr_frag.spv");
            std::shared_ptr<vk::VulkanTexture> texture = LoadTexture("assets/textures/white.png");
            std::array<vk::MaterialData, MAX_MATERIAL_DATA> material_data;

            tinygltf::Model model;
            tinygltf::TinyGLTF gltf_ctx;
            std::string err;
            std::string warn;
            bool ret = false;

            ret = gltf_ctx.LoadBinaryFromFile(&model, &err, &warn, path.c_str());
            if (!warn.empty()) {
                printf("Warn: %s\n", warn.c_str());
            }
            if (!err.empty()) {
                printf("Err: %s\n", err.c_str());
            }
            if (!ret) {
                printf("Failed to parse glTF\n");
            }

            for (int i = 0; i < model.materials.size(); i++) {
                auto color_vect = model.materials[i].pbrMetallicRoughness.baseColorFactor;
                glm::vec3 albedo;
                // TODO transparent materials in albedo
                for (int c = 0; c < 3; c++) {
                    albedo[c] = color_vect[c];
                }
                material_data[i].albedo = albedo;
                material_data[i].roughness = model.materials[i].pbrMetallicRoughness.metallicFactor;
                material_data[i].metallic = model.materials[i].pbrMetallicRoughness.roughnessFactor;
                material_data[i].ao = 0.1f;
            }
            m_materials[path] = std::make_shared<vk::VulkanMaterial>(vertex_shader, fragment_shader, texture, material_data);
        }
    }
    return m_materials[path];
}
