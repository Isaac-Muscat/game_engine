#pragma once
#include "Core.h"
#include "hid/Window.h"
#include "Types.h"
#include "Swapchain.h"

#include "vulkan/vulkan.h"

namespace vk::init {

	VkInstance createInstance(bool enableValidationLayers);
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