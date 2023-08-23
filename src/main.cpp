#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "Core.h"
#include "renderer/vk/Renderer.h"
#include "hid/Window.h"
#include "renderer/vk/Init.h"

int main() {
    //// Global systems initialization
    Window window(800, 600, "Game Engine");
    vk::gRenderer.init(window);

    while (!window.ShouldClose()) {
        window.PollEvents();
    }

    // Global systems destruction
    vk::gRenderer.destroy();
    return EXIT_SUCCESS;
}