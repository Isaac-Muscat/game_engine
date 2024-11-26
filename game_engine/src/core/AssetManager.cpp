#include "pch.h"
#include "renderer/vk/VulkanRenderer.h"
#include "AssetManager.h"

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
    }
    return m_materials[path];
}
