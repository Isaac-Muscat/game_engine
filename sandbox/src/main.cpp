#include "renderer/vk/VulkanRenderer.h"
#include "hid/Windows/WindowsWindow.h"

std::unique_ptr<Renderer> g_renderer;
std::unique_ptr<Window> g_window;


int main() {
    //// Global systems initialization
    g_window = std::make_unique<WindowsWindow>(1200, 900, "Game Engine");
    g_renderer = std::make_unique<vk::VulkanRenderer>();

    while (!g_window->should_close()) {
        g_window->poll_events();
        g_renderer->draw();
    }

    // Global systems destruction
    g_renderer->destroy();
    return EXIT_SUCCESS;
}