#include "renderer/vk/VulkanRenderer.h"
#include "hid/win32/Win32Window.h"

std::unique_ptr<Renderer> g_renderer;
std::unique_ptr<Window> g_window;


int main() {
    //// Global systems initialization
    g_window = std::make_unique<win32::Win32Window>(1200, 900, "Game Engine");
    g_renderer = std::make_unique<vk::VulkanRenderer>();

    while (!g_window->ShouldClose()) {
        g_window->PollEvents();
        g_renderer->Draw();
    }

    // Global systems destruction
    g_renderer->Destroy();
    return EXIT_SUCCESS;
}