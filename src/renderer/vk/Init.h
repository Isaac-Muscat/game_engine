#pragma once
#include "Core.h"
#include "hid/Window.h"
#include "renderer/vk/Types.h"
#include "renderer/vk/Swapchain.h"

#include "vulkan/vulkan.h"

namespace vk::init {


    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
    };

	VkInstance createInstance(bool enableValidationLayers, const Window& window);
	VkPhysicalDevice createPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkSampleCountFlagBits* msaa_samples);

    VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, bool enableValidationLayers);
    VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    VkRenderPass createRenderPass(const VulkanContext& context, const std::unique_ptr<Swapchain>& swapchain);
    VkDescriptorSetLayout createDescriptorSetLayout(const VulkanContext& context);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

}