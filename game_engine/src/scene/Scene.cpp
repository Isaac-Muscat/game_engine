#include "pch.h"
#include "Scene.h"
#include "ecs/EntityScript.h"
#include "ecs/EntityComponentSystem.h"

Scene::Scene() {
    m_ecs = std::make_unique<EntityComponentSystem>();
	m_ecs->RegisterView<ScriptComponent>();
    m_ecs->RegisterView<MeshComponent, TransformComponent>();
}

void Scene::Init() {
    const auto& scripts = m_ecs->GetView<ScriptComponent>();
    for (const auto& e : scripts) {
        auto& script = m_ecs->GetComponent<ScriptComponent>(e);
        script.script->OnAwake();
    }

    for (const auto& e : scripts) {
        auto& script = m_ecs->GetComponent<ScriptComponent>(e);
        script.script->OnCreate();
    }
}

void Scene::OnUpdate() {
    const auto& scripts = m_ecs->GetView<ScriptComponent>();
    for (const auto& e : scripts) {
        auto& script = m_ecs->GetComponent<ScriptComponent>(e);
        script.script->OnUpdate();
    }
}

void Scene::RenderScene() {
    const auto& meshes = m_ecs->GetView<MeshComponent, TransformComponent>();
    // Example API:
    // g_renderer->StartQueue(camera, environment);
    /*for (const auto& e : meshes) {
        auto& mesh = m_ecs->GetComponent<MeshComponent>(e);
        auto& transform = m_ecs->GetComponent<TransformComponent>(e);
        g_renderer->Submit(mesh, transform);
    }*/
    // g_renderer->EndQueue();
}

void Scene::Destroy() {
    const auto& scripts = m_ecs->GetView<ScriptComponent>();
    for (const auto& e : scripts) {
        auto& script = m_ecs->GetComponent<ScriptComponent>(e);
        script.script->OnDestroy();
    }
}
