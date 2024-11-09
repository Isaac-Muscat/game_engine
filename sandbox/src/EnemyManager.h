#pragma once
#include "pch.h"
#include "ecs/Components.h"
#include "ecs/EntityScript.h"
#include "hid/Input.h"
#include "core/Time.h"
#include "core/SimplexNoise.h"
#include "physics/TriangleBVH.h"

class EnemyManager : public EntityScript {
public:
    std::vector<Entity> Enemies;

    void OnAwake() override {
        white_texture = std::make_shared<vk::VulkanTexture>("assets/textures/white.png");
        white_material = std::make_shared<vk::VulkanMaterial>(white_texture);

        for (int x = -w; x <= w; x++) {
            Entity terrain = CreateEntity();
            terrain.AddComponent<TransformComponent>({ glm::vec3(x * size, -31.0f, z * size), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1)});
            terrain.AddComponent<MaterialComponent>({ white_material });
            MakeTerrain(size, size, x, z, terrain);
            terrain.GetComponent<MeshColliderComponent>().bvh.m_position = terrain.GetComponent<TransformComponent>().position;
            chunks.push_back(terrain);
        }
    }

    // Need to make way more efficent so vector doesnt copy all data over and over again
    // TODO: Need to make not lag on load chunks
	void OnUpdate() override {
        glm::vec3 player_pos = player_entity.GetComponent<TransformComponent>().position;

        if (floor(player_pos.x / size) > current_chunk_x) {
            current_chunk_x = floor(player_pos.x / size);
            for (int i = 0; i < chunks.size(); i++) {
                auto t = chunks[i]; 
                glm::vec3 t_pos = t.GetComponent<TransformComponent>().position;
                if (floor(t_pos.x / size) < current_chunk_x - w) {
                    chunks[i].Destroy();
                    Entity terrain = CreateEntity();
                    terrain.AddComponent<TransformComponent>({ glm::vec3((current_chunk_x + w) * size, -31.0f, t_pos.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1)});
                    MakeTerrain(size, size, current_chunk_x + w, floor(t_pos.z/size), terrain);
                    terrain.AddComponent<MaterialComponent>({ white_material });
                    terrain.GetComponent<MeshColliderComponent>().bvh.m_position = terrain.GetComponent<TransformComponent>().position;
                    chunks[i] = terrain;
                }
            }
        } else if (floor(player_pos.x / size) < current_chunk_x) {
            current_chunk_x = floor(player_pos.x / size);
            for (int i = 0; i < chunks.size(); i++) {
                auto t = chunks[i]; 
                glm::vec3 t_pos = t.GetComponent<TransformComponent>().position;
                if (floor(t_pos.x / size) > current_chunk_x + w) {
                    chunks[i].Destroy();
                    Entity terrain = CreateEntity();
                    terrain.AddComponent<TransformComponent>({ glm::vec3((current_chunk_x - w) * size, -31.0f, t_pos.z), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1)});
                    MakeTerrain(size, size, current_chunk_x - w, floor(t_pos.z/size), terrain);
                    terrain.AddComponent<MaterialComponent>({ white_material });
                    terrain.GetComponent<MeshColliderComponent>().bvh.m_position = terrain.GetComponent<TransformComponent>().position;
                    chunks[i] = terrain;
                }
            }
        }

        if (floor(player_pos.z / size) > current_chunk_z) {
            current_chunk_z = floor(player_pos.z / size);
            for (int i = 0; i < chunks.size(); i++) {
                auto t = chunks[i]; 
                glm::vec3 t_pos = t.GetComponent<TransformComponent>().position;
                if (floor(t_pos.z / size) < current_chunk_z - w) {
                    chunks[i].Destroy();
                    Entity terrain = CreateEntity();
                    terrain.AddComponent<TransformComponent>({ glm::vec3(t_pos.x, -31.0f, (current_chunk_z + w) * size), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1)});
                    MakeTerrain(size, size, floor(t_pos.x/size), current_chunk_z + w, terrain);
                    terrain.AddComponent<MaterialComponent>({ white_material });
                    terrain.GetComponent<MeshColliderComponent>().bvh.m_position = terrain.GetComponent<TransformComponent>().position;
                    chunks[i] = terrain;
                }
            }
        } else if (floor(player_pos.z / size) < current_chunk_z) {
            current_chunk_z = floor(player_pos.z / size);
            for (int i = 0; i < chunks.size(); i++) {
                auto t = chunks[i]; 
                glm::vec3 t_pos = t.GetComponent<TransformComponent>().position;
                if (floor(t_pos.z / size) > current_chunk_z + w) {
                    chunks[i].Destroy();
                    Entity terrain = CreateEntity();
                    terrain.AddComponent<TransformComponent>({ glm::vec3(t_pos.x, -31.0f, (current_chunk_z - w) * size), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1)});
                    MakeTerrain(size, size, floor(t_pos.x/size), current_chunk_z - w, terrain);
                    terrain.AddComponent<MaterialComponent>({ white_material });
                    terrain.GetComponent<MeshColliderComponent>().bvh.m_position = terrain.GetComponent<TransformComponent>().position;
                    chunks[i] = terrain;
                }
            }
        }
	};
};
