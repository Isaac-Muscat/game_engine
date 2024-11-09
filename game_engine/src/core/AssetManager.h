#pragma once
#include "pch.h"
#include "renderer/vk/VulkanMaterial.h"
#include "renderer/vk/VulkanMesh.h"
#include "renderer/vk/VulkanTexture.h"

typedef uint64_t uuid;

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
    std::unordered_map<uuid, vk::VulkanTexture> m_textures;
    std::unordered_map<uuid, vk::VulkanMesh> m_meshes;
    std::unordered_map<uuid, vk::VulkanMaterial> m_material;
public:
    AssetManager(){}

};
