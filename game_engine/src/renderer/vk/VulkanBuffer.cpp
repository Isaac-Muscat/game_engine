#include "pch.h"
#include "VulkanInit.h"
#include "VulkanTypes.h"
#include "VulkanBuffer.h"

namespace vk {
	VulkanDeviceBuffer::VulkanDeviceBuffer(const VulkanContext& context, VkBufferUsageFlags buffer_type,void* host_data, size_t size_bytes) 
        : m_size_bytes(size_bytes) {
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        init::CreateBuffer(context, size_bytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

        void* device_data;
        vkMapMemory(context.device, staging_buffer_memory, 0, size_bytes, 0, &device_data);
        memcpy(device_data, host_data, size_bytes);
        vkUnmapMemory(context.device, staging_buffer_memory);

        init::CreateBuffer(context, size_bytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT | buffer_type, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_memory);
        CopyBuffer(context, staging_buffer, m_buffer, size_bytes);

        vkDestroyBuffer(context.device, staging_buffer, nullptr);
        vkFreeMemory(context.device, staging_buffer_memory, nullptr);
	}

    void VulkanDeviceBuffer::CopyBuffer(const VulkanContext& context, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
        VkCommandBuffer command_buffer = init::BeginSingleTimeCommands(context);

        VkBufferCopy copy_region{};
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

        init::EndSingleTimeCommands(context, command_buffer);
    }

    VulkanSharedBuffer::VulkanSharedBuffer(const VulkanContext& context, VkBufferUsageFlags buffer_type, size_t size_bytes) 
        : m_size_bytes(size_bytes) {
        init::CreateBuffer(context, size_bytes, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_buffer, m_memory);
    }

    void VulkanSharedBuffer::UpdateData(const VulkanContext& context, void* host_data, size_t size_bytes) {
        void* device_data;
        vkMapMemory(context.device, m_memory, 0, size_bytes, 0, &device_data);
        memcpy(device_data, host_data, size_bytes);
        vkUnmapMemory(context.device, m_memory);
    }
}