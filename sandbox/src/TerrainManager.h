#pragma once
#include "pch.h"
#include "ecs/Components.h"
#include "ecs/EntityScript.h"
#include "hid/Input.h"
#include "core/Time.h"
#include "core/SimplexNoise.h"
#include "physics/TriangleBVH.h"

class TerrainManager : public EntityScript {
public:
    std::vector<Entity> chunks;
    Entity player_entity;
    SimplexNoise noise = SimplexNoise(1, 1, 2, 0.5);
    int size = 20;
    int current_chunk_x = 0;
    int current_chunk_z = 0;
    int w = 3;
    std::shared_ptr<vk::VulkanTexture> white_texture;
    std::shared_ptr<vk::VulkanMaterial> white_material;

    Entity GetClosestChunk(glm::vec3 pos) {
        for (auto chunk: chunks) {
            glm::vec3 chunk_pos = chunk.GetComponent<TransformComponent>().position;
            if (current_chunk_x == floor(chunk_pos.x / size) && current_chunk_z == floor(chunk_pos.z / size)) {
                return chunk;
            }
        }
        return chunks[0];
    }

    glm::vec3 calculate_normal(std::array<glm::vec3, 3> v) {
        glm::vec3 l1 = v[0] - v[1];
        glm::vec3 l2 = v[0] - v[2];
        return glm::normalize(glm::cross(l1, l2));
    }

    void MakeTerrain(int size_x, int size_z, int x_offset, int z_offset, Entity terrain) {

        std::vector<vk::Vertex> vertices;
        std::vector<uint32_t> indices;
        TriangleBVH bvh;

        // Create all vertices (there are duplicates to fix normals)
        // Each inner loop creates two triangles to make a quad (6 vertices each)
        int octaves = 4;
        float p_step = 1.0f;
        float n_step = 0.007f;
        float h = 30;
        int index = 0;
        for (int i_x = 0; i_x < size_x; i_x++) {
            for (int i_z = 0; i_z < size_z; i_z++) {
                float n_x = (i_x + size_x * x_offset) * n_step;
                float n_z = (i_z + size_z * z_offset) * n_step;

                float p_x = i_x * p_step;
                float p_z = i_z * p_step;
                glm::vec3 color(0.3f, 0.8f, 0.2f);

                // TODO Fix normals!!
                vk::Vertex v1, v2, v3;
                v1 = { glm::vec3(p_x,         noise.fractal(octaves, n_x, n_z) * h,               p_z       ), color, glm::vec2(0), glm::vec3(0, 1, 0) };
                v2 = { glm::vec3(p_x,         noise.fractal(octaves, n_x, n_z + n_step) * h,        p_z + p_step), color, glm::vec2(0), glm::vec3(0, 1, 0) };
                v3 = { glm::vec3(p_x + p_step,  noise.fractal(octaves, n_x + n_step, n_z + n_step) * h, p_z + p_step), color, glm::vec2(0), glm::vec3(0, 1, 0) };
                glm::vec3 n = calculate_normal({v1.pos, v2.pos, v3.pos});
                v1.normal = n;
                v2.normal = n;
                v3.normal = n;
                bvh.InsertTriangle({v1.pos, v2.pos, v3.pos});
                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(v3);
                indices.push_back(index);
                indices.push_back(index + 1);
                indices.push_back(index + 2);
                
                index += 3;

                vk::Vertex v4, v5, v6;
                v4 = { glm::vec3(p_x,           noise.fractal(octaves, n_x, n_z) * h,               p_z       ), color, glm::vec2(0), glm::vec3(0, 1, 0) };
                v5 = { glm::vec3(p_x + p_step,  noise.fractal(octaves, n_x + n_step, n_z + n_step) * h, p_z + p_step), color, glm::vec2(0), glm::vec3(0, 1, 0) };
                v6 = { glm::vec3(p_x + p_step,  noise.fractal(octaves, n_x + n_step, n_z) * h,        p_z       ), color, glm::vec2(0), glm::vec3(0, 1, 0) };
                n = calculate_normal({v4.pos, v5.pos, v6.pos});
                v4.normal = n;
                v5.normal = n;
                v6.normal = n;
                vertices.push_back(v4);
                vertices.push_back(v5);
                vertices.push_back(v6);
                bvh.InsertTriangle({v4.pos, v5.pos, v6.pos});
                indices.push_back(index);
                indices.push_back(index + 1);
                indices.push_back(index + 2);
                
                index += 3;
            }
        }
        terrain.AddComponent<MeshColliderComponent>({bvh, true, false});
        terrain.AddComponent<MeshComponent>({std::make_shared<vk::VulkanMesh>(vertices, indices)});
    }

    void OnAwake() override {
        player_entity = FindEntityByTag("main_player");
        white_texture = std::make_shared<vk::VulkanTexture>("assets/textures/white.png");
        white_material = std::make_shared<vk::VulkanMaterial>(white_texture);

        for (int x = -w; x <= w; x++) {
            for (int z = -w; z <= w; z++) {
                Entity terrain = CreateEntity();
                terrain.AddComponent<TransformComponent>({ glm::vec3(x * size, -31.0f, z * size), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1)});
                terrain.AddComponent<MaterialComponent>({ white_material });
                MakeTerrain(size, size, x, z, terrain);
                terrain.GetComponent<MeshColliderComponent>().bvh.m_position = terrain.GetComponent<TransformComponent>().position;
                chunks.push_back(terrain);
            }
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
