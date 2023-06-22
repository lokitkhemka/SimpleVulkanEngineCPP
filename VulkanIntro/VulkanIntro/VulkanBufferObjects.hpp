#pragma once

#include "VulkanDevice.hpp"

namespace vlkn {
	class VulkanBufferObjects
	{
	public:
		static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

		VulkanBufferObjects(VulkanDevice& device, VkDeviceSize instanceSize, uint32_t instanceCount,
			VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment = 1);

		~VulkanBufferObjects();

		VulkanBufferObjects(const VulkanBufferObjects&) = delete;
		VulkanBufferObjects& operator=(const VulkanBufferObjects&) = delete;

		VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void Unmap();

		void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		void WriteToIndex(void* data, int index);
		VkResult FlushIndex(int index);
		VkDescriptorBufferInfo DescriptorInfoForIndex(int index);
		VkResult InvalidateIndex(int index);

		VkBuffer GetBuffer() const { return Buffer; }
		void* GetMappedMemory() const { return Mapped; }
		uint32_t GetInstanceCount() const { return InstanceCount; }
		VkDeviceSize GetInstanceSize() const { return InstanceSize; }
		VkMemoryPropertyFlags GetMemoryPropertyFlags() const { return MemoryPropertyFlags; }
		VkDeviceSize GetBufferSize() const { return BufferSize; }

	private:
		VulkanDevice& Device;
		void* Mapped = nullptr;
		VkBuffer Buffer = VK_NULL_HANDLE;
		VkDeviceMemory Memory = VK_NULL_HANDLE;

		VkDeviceSize BufferSize;
		uint32_t InstanceCount;
		VkDeviceSize InstanceSize;
		VkDeviceSize AlignmentSize;
		VkBufferUsageFlags UsageFlags;
		VkMemoryPropertyFlags MemoryPropertyFlags;
	};

}