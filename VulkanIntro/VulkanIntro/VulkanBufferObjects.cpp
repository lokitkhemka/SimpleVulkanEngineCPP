#include "VulkanBufferObjects.hpp"

#include<cassert>
#include<cstring>


namespace vlkn {

	/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
	VkDeviceSize VulkanBufferObjects::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
	{
		if (minOffsetAlignment > 0)
		{
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		return instanceSize;
	}
	VulkanBufferObjects::VulkanBufferObjects(VulkanDevice &device, VkDeviceSize instanceSize, uint32_t instanceCount, 
											VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags
											, VkDeviceSize minOffsetAlignment)
		: Device{device},
		InstanceSize{instanceSize},
		InstanceCount{instanceCount},
		UsageFlags{usageFlags},
		MemoryPropertyFlags{memoryPropertyFlags}
	{
		AlignmentSize = GetAlignment(InstanceSize, minOffsetAlignment);
		BufferSize = AlignmentSize * InstanceCount;
		Device.createBuffer(BufferSize, UsageFlags, MemoryPropertyFlags, Buffer, Memory);
	}

	VulkanBufferObjects::~VulkanBufferObjects()
	{
		Unmap();
		vkDestroyBuffer(Device.device(), Buffer, nullptr);
		vkFreeMemory(Device.device(), Memory, nullptr);
	}

	/**
 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
 *
 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
 * buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the buffer mapping call
 */
	VkResult VulkanBufferObjects::Map(VkDeviceSize size, VkDeviceSize offset)
	{
		assert(Buffer && Memory && "Called Map on Buffer Before Creating the Buffer.");
		return vkMapMemory(Device.device(), Memory, offset, size, 0, &Mapped);
	}

	/**
 * Unmap a mapped memory range
 *
 * @note Does not return a result as vkUnmapMemory can't fail
 */
	void VulkanBufferObjects::Unmap()
	{
		if (Mapped)
		{
			vkUnmapMemory(Device.device(), Memory);
			Mapped = nullptr;
		}
	}

	/**
 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
 *
 * @param data Pointer to the data to copy
 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
 * range.
 * @param offset (Optional) Byte offset from beginning of mapped region
 *
 */
	void VulkanBufferObjects::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
	{
		assert(Mapped && "Cannot Copy to Unmapped Buffer");

		if (size == VK_WHOLE_SIZE)
		{
			memcpy(Mapped, data, BufferSize);
		}
		else
		{
			char* memOffset = (char*)Mapped;
			memOffset += offset;
			memcpy(memOffset, data, size);
		}
	}

	/**
 * Flush a memory range of the buffer to make it visible to the device
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
 * complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the flush call
 */

	VkResult VulkanBufferObjects::Flush(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = Memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(Device.device(), 1, &mappedRange);
	}

	/**
 * Create a buffer info descriptor
 *
 * @param size (Optional) Size of the memory range of the descriptor
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkDescriptorBufferInfo of specified offset and range
 */

	VkDescriptorBufferInfo VulkanBufferObjects::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
	{
		return VkDescriptorBufferInfo{Buffer, offset, size};
	}


	/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
 * the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the invalidate call
 */
	VkResult VulkanBufferObjects::Invalidate(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = Memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(Device.device(), 1, &mappedRange);
	}

	/**
 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
 *
 * @param data Pointer to the data to copy
 * @param index Used in offset calculation
 *
 */
	void VulkanBufferObjects::WriteToIndex(void* data, int index)
	{
		WriteToBuffer(data, InstanceSize, index * AlignmentSize);
	}

	/**
 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
 *
 * @param index Used in offset calculation
 *
 */

	VkResult VulkanBufferObjects::FlushIndex(int index)
	{
		return Flush(AlignmentSize, index * AlignmentSize);
	}

	/**
 * Create a buffer info descriptor
 *
 * @param index Specifies the region given by index * alignmentSize
 *
 * @return VkDescriptorBufferInfo for instance at index
 */

	VkDescriptorBufferInfo VulkanBufferObjects::DescriptorInfoForIndex(int index)
	{
		return DescriptorInfo(AlignmentSize, index * AlignmentSize);
	}

	/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param index Specifies the region to invalidate: index * alignmentSize
 *
 * @return VkResult of the invalidate call
 */

	VkResult VulkanBufferObjects::InvalidateIndex(int index)
	{
		return Invalidate(AlignmentSize, index * AlignmentSize);
	}


}
