#pragma once
#include "pch.h"
#include "glm/glm.hpp"
#include <vulkan/vulkan.h>

namespace vk {
    struct UniformBufferObject {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    // Struct for passing around commonly needed information
    typedef struct VulkanContext {
        const int MAX_FRAMES_IN_FLIGHT = 2;
        VkInstance instance;
        VkSurfaceKHR surface;
        VkPhysicalDevice physical_device;
        VkDevice device;
        VkQueue graphics_queue;
        VkQueue present_queue;
        VkCommandPool command_pool;

        VkSampleCountFlagBits msaa_samples;
        VkDebugUtilsMessengerEXT debug_messenger;
        bool enable_validation_layers;
    } VulkanContext;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

}
