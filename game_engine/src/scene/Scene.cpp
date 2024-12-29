#include "ecs/Components.h"
#include "pch.h"
#include "Scene.h"
#include "ecs/EntityScript.h"
#include "ecs/EntityComponentSystem.h"
#include "renderer/Camera.h"
#include "renderer/vk/VulkanMaterial.h"
#include "renderer/vk/VulkanRenderer.h"
#include "core/Time.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene() {
    m_ecs = std::make_unique<EntityComponentSystem>();
    m_ecs->RegisterView<MeshComponent, TransformComponent, MaterialComponent>();
    m_ecs->RegisterView<LightComponent, TransformComponent>();
    m_ecs->RegisterView<TransformComponent, AABBComponent>();
}

void Scene::Init() {
    const auto& scripts = m_ecs->GetComponentArray<ScriptComponent>();
    for (size_t i = 0; i < scripts.End(); i++) {
        auto& script = scripts[i];
        script.script->OnAwake();
    }

    for (size_t i = 0; i < scripts.End(); i++) {
        auto& script = scripts[i];
        script.script->OnCreate();
    }
}

// FIXME!!! Camera -> make use TransformComponent angles!!
void Scene::CollisionUpdate() {
    const auto& aabbs = m_ecs->GetView<TransformComponent, AABBComponent>();
    for (const auto& a1 : aabbs) {
        AABBComponent& aabb_component1 = m_ecs->GetComponent<AABBComponent>(a1);
        TransformComponent& transform1 = m_ecs->GetComponent<TransformComponent>(a1);
        aabb_component1.collided = false;
        for (const auto& a2 : aabbs) {
            if (a2 == a1) { continue; }
            AABBComponent& aabb_component2 = m_ecs->GetComponent<AABBComponent>(a2);
            TransformComponent& transform2 = m_ecs->GetComponent<TransformComponent>(a2);

            AABB aabb1 = aabb_component1.aabb;
            aabb1.Translate(transform1.position);
            AABB aabb2 = aabb_component2.aabb;
            aabb2.Translate(transform2.position);

            if (AABB::Collision(aabb1, aabb2)) {
                aabb_component1.collided = true;
            }

        }
    }
}

void Scene::OnUpdate() {
    const auto& scripts = m_ecs->GetComponentArray<ScriptComponent>();
    for (size_t i = 0; i < scripts.End(); i++) {
        auto& script = scripts[i];
        script.script->OnUpdate();
    }
}

void Scene::RenderScene() {






    // Submit Meshes
    const auto& entities = m_ecs->GetView<MeshComponent, TransformComponent, MaterialComponent>();
    const auto& lights = m_ecs->GetView<LightComponent, TransformComponent>();
    // TODO: Make Efficient!
    std::vector<Light> light_vec;
    for (const auto& e : lights) {
        LightComponent& light_comp = m_ecs->GetComponent<LightComponent>(e);
        TransformComponent& transform = m_ecs->GetComponent<TransformComponent>(e);
        Light light{};
        light.type = light_comp.type;
        light.color = light_comp.color;
        light.ambient = light_comp.ambient;
        light.diffuse = light_comp.diffuse;
        light.specular = light_comp.specular;
        light.position = transform.position;
        light.direction = {
            cos(transform.euler_angles.y)*cos(transform.euler_angles.z),
            sin(transform.euler_angles.y)*cos(transform.euler_angles.z),
            sin(transform.euler_angles.z)
        };
        light_vec.push_back(light);
    }
    // TODO Do some sorting and frustrum culling
    std::vector<Entity> entities_sorted;
    for (const auto& e : entities) {
        entities_sorted.push_back(Entity(e, m_ecs.get()));
    }

    struct less_than {
        inline bool operator() (const Entity& e1, const Entity& e2) {
            auto mat1 = e1.GetComponent<MaterialComponent>();
            auto mat2 = e2.GetComponent<MaterialComponent>();
            uint64_t val1 = (uint64_t)mat1.material->m_pipeline.pipeline;
            uint64_t val2 = (uint64_t)mat2.material->m_pipeline.pipeline;
            uint64_t mdat1 = (uint64_t)mat1.material->m_material_buffer->m_buffer;
            uint64_t mdat2 = (uint64_t)mat2.material->m_material_buffer->m_buffer;
            return val1 < val2 || mdat1 < mdat2;
        }
    };
    //std::sort(entities_sorted.begin(), entities_sorted.end(), less_than());

    float update_time = Stopwatch::Stop();
    Stopwatch::Start();
    g_renderer->BeginFrame(m_main_camera.GetComponent<CameraComponent>().camera, light_vec);
    // Final submission of all meshes
    for (const auto& e : entities_sorted) {
        MeshComponent& mesh = m_ecs->GetComponent<MeshComponent>(e.GetID());
        TransformComponent& transform = m_ecs->GetComponent<TransformComponent>(e.GetID());
        MaterialComponent& material = m_ecs->GetComponent<MaterialComponent>(e.GetID());
        //std::cout << material.material->m_pipeline.pipeline << std::endl;
        //std::cout << material.material->m_material_data[0].albedo.x << " " << material.material->m_material_data[0].albedo.y << " " << material.material->m_material_data[0].albedo.z << std::endl;
        // glm::rotate(glm::mat4(1.0f), Time::DeltaTime() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f))
        g_renderer->Draw(mesh.mesh, transform.GetModelMatrix(), material.material);
    }
    g_renderer->EndFrame();
    float render_time = Stopwatch::Stop();
    std::cout << "Update Time (ms): " << update_time << " " << 
                 "Render Time (ms): " << render_time << "\n";
}

void Scene::Destroy() {
    const auto& scripts = m_ecs->GetComponentArray<ScriptComponent>();
    for (size_t i = 0; i < scripts.End(); i++) {
        auto& script = scripts[i];
        script.script->OnDestroy();
    }
}
