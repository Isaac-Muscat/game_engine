#include "pch.h"
#include "Win32Window.h"

#include "vulkan/vulkan.h"

namespace win32 {
    Win32Window::Win32Window(int width, int height, std::string name)
        : m_width(width), m_height(height), m_name(name) {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(width, height, m_name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);
    }

    Win32Window::~Win32Window()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    std::vector<const char*>  Win32Window::GetRequiredExtensions(bool enable_validation_layers) const {
        uint32_t extension_count = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&extension_count);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extension_count);

        if (enable_validation_layers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void Win32Window::CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* p_surface) const {
        VkResult result = glfwCreateWindowSurface(instance, m_window, nullptr, p_surface);
        assert(result == VK_SUCCESS && "failed to create window surface!");
    }

    void Win32Window::GetFrameBufferSize(int* width, int* height) {
        glfwGetFramebufferSize(m_window, width, height);
    }
    void* Win32Window::GetNativeWindowPointer()
    {
        return m_window;
    }
}
