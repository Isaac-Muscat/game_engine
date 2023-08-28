#include "WindowsWindow.h"
#include "vulkan/vulkan.h"

WindowsWindow::WindowsWindow(int width, int height, std::string name)
    : mWidth(width), mHeight(height), mName(name) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    mWindow = glfwCreateWindow(width, height, mName.c_str(), nullptr, nullptr);
}

WindowsWindow::~WindowsWindow()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

std::vector<const char*>  WindowsWindow::get_required_extensions(bool enableValidationLayers) const {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void WindowsWindow::create_vulkan_surface(VkInstance instance, VkSurfaceKHR* pSurface) const {
    assert(glfwCreateWindowSurface(instance, mWindow, nullptr, pSurface) == VK_SUCCESS && "failed to create window surface!");
}

void WindowsWindow::get_frame_buffer_size(int* width, int* height) {
    glfwGetFramebufferSize(mWindow, width, height);
}
