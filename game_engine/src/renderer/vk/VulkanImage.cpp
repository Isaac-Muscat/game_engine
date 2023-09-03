#include "pch.h"
#include "VulkanImage.h"
#include "VulkanInit.h"

namespace vk {
    VulkanImage::VulkanImage(const VulkanContext& context, uint32_t width, uint32_t height, uint32_t mip_levels, VkSampleCountFlagBits num_samples, VkFormat format,
        VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect_flags, VkMemoryPropertyFlags properties) 
        : m_context(context) {
    
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = width;
        image_info.extent.height = height;
        image_info.extent.depth = 1;
        image_info.mipLevels = mip_levels;
        image_info.arrayLayers = 1;
        image_info.format = format;
        image_info.tiling = tiling;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = usage;
        image_info.samples = num_samples;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(context.device, &image_info, nullptr, &m_image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements(context.device, m_image, &memory_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = memory_requirements.size;
        alloc_info.memoryTypeIndex = init::FindMemoryType(context, memory_requirements.memoryTypeBits, properties);

        if (vkAllocateMemory(context.device, &alloc_info, nullptr, &m_memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(context.device, m_image, m_memory, 0);

        VkImageViewCreateInfo view_info
        {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = m_image;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = format;
        view_info.subresourceRange.aspectMask = aspect_flags;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = mip_levels;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(context.device, &view_info, nullptr, &m_view) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    VulkanImage::~VulkanImage() {
        vkDestroyImageView(m_context.device, m_view, nullptr);
        vkDestroyImage(m_context.device, m_image, nullptr);
        vkFreeMemory(m_context.device, m_memory, nullptr);
    }
}
