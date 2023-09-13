#include "pch.h"
#include "Scene.h"
#include "ecs/EntityScript.h"
#include "ecs/EntityComponentSystem.h"
#include "renderer/Camera.h"
#include "renderer/vk/VulkanRenderer.h"
#include "core/Time.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Scene::Scene() {
    m_ecs = std::make_unique<EntityComponentSystem>();
    m_ecs->RegisterView<MeshComponent, TransformComponent, MaterialComponent>();
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

void Scene::OnUpdate() {
    const auto& scripts = m_ecs->GetComponentArray<ScriptComponent>();
    for (size_t i = 0; i < scripts.End(); i++) {
        auto& script = scripts[i];
        script.script->OnUpdate();
    }
}

void Scene::RenderScene() {
    const auto& entities = m_ecs->GetView<MeshComponent, TransformComponent, MaterialComponent>();

    g_renderer->BeginFrame(m_main_camera.GetComponent<CameraComponent>().camera);
    for (const auto& e : entities) {
        MeshComponent& mesh = m_ecs->GetComponent<MeshComponent>(e);
        TransformComponent& transform = m_ecs->GetComponent<TransformComponent>(e);
        MaterialComponent& material = m_ecs->GetComponent<MaterialComponent>(e);
        // glm::rotate(glm::mat4(1.0f), Time::DeltaTime() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f))
        g_renderer->Draw(mesh.mesh, transform.GetModelMatrix(), material.material);
    }
    g_renderer->EndFrame();
}

void Scene::Destroy() {
    const auto& scripts = m_ecs->GetComponentArray<ScriptComponent>();
    for (size_t i = 0; i < scripts.End(); i++) {
        auto& script = scripts[i];
        script.script->OnDestroy();
    }
}
