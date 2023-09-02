#pragma once
#include "pch.h"
#include "Types.h"
#include "vulkan/vulkan.h"

namespace vk {
	// TODO: Combine classes into one buffer class with option for type of access.

	// Buffer data moved from host to device.
	class DeviceBuffer {
	public:
		VkBuffer m_buffer;
		VkDeviceMemory m_memory;
		DeviceBuffer(const VulkanContext& context, VkBufferUsageFlags buffer_type, void* host_data, size_t size_bytes);
		void copy_buffer(const VulkanContext& context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	};

	// Buffer data consistantly updated by 
	class SharedBuffer {
	public:
		VkBuffer m_buffer;
		VkDeviceMemory m_memory;
		SharedBuffer(const VulkanContext& context, VkBufferUsageFlags buffer_type, size_t size_bytes);
		void update_data(const VulkanContext& context, void* host_data, size_t size_bytes);
	};
}