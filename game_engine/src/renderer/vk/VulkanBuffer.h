#pragma once
#include "pch.h"
#include "VulkanTypes.h"

#include "vulkan/vulkan.h"

// TODO: Combine classes into one buffer class with option for type of access.
namespace vk {
	// Buffer data moved from host to device.
	class VulkanDeviceBuffer {
	public:
		VkBuffer m_buffer;
		VkDeviceMemory m_memory;
		size_t m_size_bytes;
		VulkanDeviceBuffer(const VulkanContext& context, VkBufferUsageFlags buffer_type, void* host_data, size_t size_bytes);
		void CopyBuffer(const VulkanContext& context, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
	};

	// Buffer data consistantly updated
	class VulkanSharedBuffer {
	public:
		VkBuffer m_buffer;
		VkDeviceMemory m_memory;
		size_t m_size_bytes;
		VulkanSharedBuffer(const VulkanContext& context, VkBufferUsageFlags buffer_type, size_t size_bytes);
		void UpdateData(const VulkanContext& context, void* host_data, size_t size_bytes);
	};
}