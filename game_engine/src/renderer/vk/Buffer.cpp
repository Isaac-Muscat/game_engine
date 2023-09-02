#include "pch.h"
#include "Init.h"
#include "Types.h"
#include "Buffer.h"

namespace vk {
	DeviceBuffer::DeviceBuffer(const VulkanContext& context, VkBufferUsageFlags buffer_type,void* host_data, size_t size_bytes) {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        init::createBuffer(context, size_bytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* device_data;
        vkMapMemory(context.device, stagingBufferMemory, 0, size_bytes, 0, &device_data);
        memcpy(device_data, host_data, size_bytes);
        vkUnmapMemory(context.device, stagingBufferMemory);

        init::createBuffer(context, size_bytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT | buffer_type, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_memory);
        copy_buffer(context, stagingBuffer, m_buffer, size_bytes);

        vkDestroyBuffer(context.device, stagingBuffer, nullptr);
        vkFreeMemory(context.device, stagingBufferMemory, nullptr);
	}

    void DeviceBuffer::copy_buffer(const VulkanContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = init::beginSingleTimeCommands(context);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        init::endSingleTimeCommands(context, commandBuffer);
    }

    SharedBuffer::SharedBuffer(const VulkanContext& context, VkBufferUsageFlags buffer_type, size_t size_bytes) {
        init::createBuffer(context, size_bytes, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_buffer, m_memory);
    }

    void SharedBuffer::update_data(const VulkanContext& context, void* host_data, size_t size_bytes) {
        void* device_data;
        vkMapMemory(context.device, m_memory, 0, size_bytes, 0, &device_data);
        memcpy(device_data, host_data, size_bytes);
        vkUnmapMemory(context.device, m_memory);
    }
}