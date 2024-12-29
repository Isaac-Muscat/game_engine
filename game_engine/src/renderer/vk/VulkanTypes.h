#pragma once
#include "pch.h"
#include "glm/glm.hpp"
#include <vulkan/vulkan.h>

namespace vk {
    struct UniformBufferObject {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    struct ModelPushConstant {
        alignas(16) glm::mat4 proj;
    };

    struct RenderPipeline {
        VkPipeline pipeline;
        VkPipelineLayout pipeline_layout;
    };

    // Struct for passing around commonly needed information
    typedef struct VulkanContext {
        const int MAX_FRAMES_IN_FLIGHT = 3;
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
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;

        bool IsComplete() {
            return graphics_family.has_value() && present_family.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

}
