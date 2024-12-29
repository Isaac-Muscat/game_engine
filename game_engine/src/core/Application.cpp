#include "pch.h"
#include "Application.h"
#include "renderer/vk/VulkanRenderer.h"
#include "hid/windows/Win32Window.h"
#include "core/AssetManager.h"
#include "ecs/EntityComponentSystem.h"
#include "ecs/EntityScript.h"
#include "core/Time.h"

std::unique_ptr<vk::VulkanRenderer> g_renderer;
std::unique_ptr<Window> g_window;
std::unique_ptr<AssetManager> g_asset_manager;

// Initialize all engine singleton systems
Application::Application(ApplicationInfo info) {
    std::cout << "INIT" << std::endl;
    g_asset_manager = std::make_unique<AssetManager>();

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

    OnCreate();
}

Application::~Application()
{
    OnDestroy();
}

void Application::Run() {
    bool scene_loaded = m_scenes.size() > 0;
    
    if (scene_loaded) {
        m_scenes[m_current_scene_index]->Init();
    }

    while (!g_window->ShouldClose() && !m_user_says_close) {
        Time::Tick();
        g_renderer->GetNextBuffer();
        g_window->PollEvents();

        Stopwatch::Start();
        if (scene_loaded) {
            m_scenes[m_current_scene_index]->OnUpdate();
            m_scenes[m_current_scene_index]->CollisionUpdate();
        }
        OnUpdate();

        if (scene_loaded) {
            m_scenes[m_current_scene_index]->RenderScene();
        }
    }

    if (scene_loaded) {
        m_scenes[m_current_scene_index]->Destroy();
    }
}
