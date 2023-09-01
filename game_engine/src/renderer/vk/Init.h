#pragma once
#include "pch.h"
#include "hid/Window.h"
#include "Types.h"
#include "Swapchain.h"
#include "Shader.h"

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
    VkRenderPass createRenderPass(const VulkanContext& context);
    VkDescriptorSetLayout createDescriptorSetLayout(const VulkanContext& context);
    VkPipeline createGraphicsPipeline(const VulkanContext& context, std::shared_ptr<Shader> shader, 
        const std::unique_ptr<Swapchain>& swapchain, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout, VkPipelineLayout* pipelineLayout);
    VkCommandPool create_commmand_pool(const VulkanContext& context);
    void createBuffer(const VulkanContext& context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    uint32_t findMemoryType(const VulkanContext& context, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkFormat find_supported_format(const VulkanContext& context, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat find_depth_format(const VulkanContext& context);

    VkCommandBuffer beginSingleTimeCommands(const VulkanContext& context);
    void endSingleTimeCommands(const VulkanContext& context, VkCommandBuffer commandBuffer);

}