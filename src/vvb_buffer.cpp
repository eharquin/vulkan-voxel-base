// vulkan base
#include "vvb_buffer.hpp"

// std
#include <cstring> 

/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instanceSize The size of an instance
 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg : minUniformBufferOffsetAlignment)
 *
 * @return VkDeviceSize of the instanvceSize with the alignment
 */
VkDeviceSize VvbBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
{
	if (minOffsetAlignment > 0)
	{
		return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
	}

	return instanceSize;
}

VvbBuffer::VvbBuffer(VvbDevice& vvbDevice, VkDeviceSize instanceSize, uint32_t instanceCount, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignement)
	: vvbDevice(vvbDevice), instanceSize(instanceSize), instanceCount(instanceCount), usageFlags(usageFlags), memoryPropertyFlags(memoryPropertyFlags)
{
	alignmentSize = getAlignment(instanceSize, minOffsetAlignement);
	bufferSize = alignmentSize * instanceCount;
	vvbDevice.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}

VvbBuffer::~VvbBuffer()
{
	unmap();
	vkDestroyBuffer(vvbDevice.getDevice(), buffer, nullptr);
	vkFreeMemory(vvbDevice.getDevice(), memory, nullptr);
}

VkResult VvbBuffer::map(VkDeviceSize size, VkDeviceSize offset)
{
	return vkMapMemory(vvbDevice.getDevice(), memory, offset, size, 0, &mapped);
}

void VvbBuffer::unmap()
{
	if (mapped)
	{
		vkUnmapMemory(vvbDevice.getDevice(), memory);
		mapped = nullptr;
	}
}

void VvbBuffer::write(void* data, VkDeviceSize size, VkDeviceSize offset)
{
	assert(mapped && "cannot write on unmap memory");

	if (size == VK_WHOLE_SIZE)
		std::memcpy(mapped, data, bufferSize);

	else
	{
		char* memOffset = (char*)mapped;
		memOffset += offset;
		std::memcpy(memOffset, data, size);
	}
}




