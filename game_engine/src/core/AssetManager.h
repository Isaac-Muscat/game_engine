#pragma once
#include "pch.h"
#include "renderer/vk/VulkanMaterial.h"
#include "renderer/vk/VulkanMesh.h"
#include "renderer/vk/VulkanShader.h"
#include "renderer/vk/VulkanTexture.h"


/*
    From Game Engine ARch
    1. Ensure one copy of resource (no reduntant data)
    2. Manage lifetime of resource
    3. load and unload resource when not needed
    4. Loading of composite/prefab? resources
    5. Manage memory usage of resource
*/

class AssetManager {
private:
    std::unordered_map<std::string, std::shared_ptr<vk::VulkanTexture>> m_textures;
    std::unordered_map<std::string, std::shared_ptr<vk::VulkanMesh>> m_meshes;
    std::unordered_map<std::string, std::shared_ptr<vk::VulkanShader>> m_shaders;
    std::unordered_map<std::string, std::shared_ptr<vk::VulkanMaterial>> m_materials;
public:
    AssetManager(){}
    std::shared_ptr<vk::VulkanMesh> LoadMesh(std::string path);
    std::shared_ptr<vk::VulkanTexture> LoadTexture(std::string path);
    std::shared_ptr<vk::VulkanMaterial> LoadMaterial(std::string path);
    std::shared_ptr<vk::VulkanShader> LoadShader(std::string path);
    // TODO Implement these assets and add them to the manager
    // Prefab LoadPrefab(std::string path);
    // Audio LoadAudio(std::string path);
    // Animation LoadAnimation(std::string path);
};

// Defined in Application along with other singletons
extern std::unique_ptr<AssetManager> g_asset_manager;

namespace Assets {
    std::shared_ptr<vk::VulkanMesh> LoadMesh(std::string path);
    std::shared_ptr<vk::VulkanTexture> LoadTexture(std::string path);
    std::shared_ptr<vk::VulkanMaterial> LoadMaterial(std::string path);
    std::shared_ptr<vk::VulkanShader> LoadShader(std::string path);
}
