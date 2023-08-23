#pragma once
#include "Core.h"

#include <vulkan/vulkan.h>

namespace vk {
    // Struct for passing around commonly needed information
    typedef struct VulkanContext {
        const int MAX_FRAMES_IN_FLIGHT = 2;
        VkInstance instance;
        VkSurfaceKHR surface;
        VkPhysicalDevice physical_device;
        VkDevice device;
        VkQueue graphics_queue;
        VkQueue present_queue;

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
