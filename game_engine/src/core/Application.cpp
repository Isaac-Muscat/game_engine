#include "pch.h"
#include "Application.h"
#include "renderer/vk/VulkanRenderer.h"
#include "hid/win32/Win32Window.h"
#include "ecs/EntityComponentSystem.h"
#include "ecs/EntityScript.h"

std::unique_ptr<Renderer> g_renderer;
std::unique_ptr<Window> g_window;
EntityComponentSystem g_ecs;

Application::Application(ApplicationInfo info) {
    switch (info.platform) {
    case Platform::Win32:
        g_window = std::make_unique<win32::Win32Window>(info.screen_width, info.screen_height, info.name);
        break;
    default:
        assert(false && "Invalid platform selected.");
        break;
    }

    switch (info.graphics_api) {
    case PlatformGraphicsAPI::Vulkan:
        g_renderer = std::make_unique<vk::VulkanRenderer>();
        break;
    default:
        assert(false && "Invalid graphics API selected.");
        break;
    }

    g_ecs.RegisterView<ScriptComponent>();

    OnCreate();
}

Application::~Application()
{
    OnDestroy();
}

void Application::Run() {
    // Later put script logic into scene class.
    g_ecs.RegisterView<ScriptComponent>();
    const auto& scripts = g_ecs.GetView<ScriptComponent>();

    for (const auto& e : scripts) {
        auto& script = g_ecs.GetComponent<ScriptComponent>(e);
        script.script->OnAwake();
    }

    for (const auto& e : scripts) {
        auto& script = g_ecs.GetComponent<ScriptComponent>(e);
        script.script->OnCreate();
    }


    while (!g_window->ShouldClose()) {
        g_window->PollEvents();
        OnUpdate();
        for (const auto& e : scripts) {
            auto& script = g_ecs.GetComponent<ScriptComponent>(e);
            script.script->OnUpdate();
        }
        g_renderer->Draw();
        g_renderer->SwapBuffers();
    }

    for (const auto& e : scripts) {
        auto& script = g_ecs.GetComponent<ScriptComponent>(e);
        script.script->OnDestroy();
    }
}
