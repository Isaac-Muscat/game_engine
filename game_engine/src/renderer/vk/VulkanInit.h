#pragma once
#include "pch.h"
#include "hid/Window.h"
#include "VulkanTypes.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"

#include "vulkan/vulkan.h"

namespace vk::init {

	VkInstance CreateInstance(bool enable_validation_layers);
	VkPhysicalDevice CreatePhysicalDevice(VkInstance instance, VkSurfaceKHR surface, VkSampleCountFlagBits* msaa_samples);
    VkDevice CreateLogicalDevice(VkPhysicalDevice physical_device, VkSurfaceKHR surface, bool enable_validation_layers);
    
    VkDescriptorSetLayout CreateDescriptorSetLayout(const VulkanContext& context);
    VkRenderPass CreateRenderPass(const VulkanContext& context);
    VkPipeline CreateGraphicsPipeline(const VulkanContext& context, std::shared_ptr<VulkanShader> shader, const std::unique_ptr<VulkanSwapchain>& swapchain, VkRenderPass renderpass, VkDescriptorSetLayout descriptor_set_layout, VkPipelineLayout* p_pipeline_layout);
    
    VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels);
    VkDescriptorPool CreateDescriptorPool(const VulkanContext& context);
    VkCommandPool CreateCommandPool(const VulkanContext& context);

    void CreateBuffer(const VulkanContext& context, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);
    void CreateSyncObjects(const VulkanContext& context, std::vector<VkSemaphore>& image_available_semaphores, std::vector<VkSemaphore>& render_finished_semaphores, std::vector<VkFence>& in_flight_fences);
    std::vector<VkCommandBuffer> CreateCommandBuffers(const VulkanContext& context);
    std::vector<VkDescriptorSet> CreateDescriptorSets(const VulkanContext& context, VkDescriptorSetLayout descriptor_set_layout, VkDescriptorPool descriptor_pool, const std::vector<std::shared_ptr<VulkanSharedBuffer>>& uniform_buffers, std::shared_ptr<VulkanTexture> texture);

    VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice physical_device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
    uint32_t FindMemoryType(const VulkanContext& context, uint32_t type_filter, VkMemoryPropertyFlags properties);
    VkFormat FindSupportedFormat(const VulkanContext& context, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat FindDepthFormat(const VulkanContext& context);

    VkCommandBuffer BeginSingleTimeCommands(const VulkanContext& context);
    void EndSingleTimeCommands(const VulkanContext& context, VkCommandBuffer command_buffer);

}