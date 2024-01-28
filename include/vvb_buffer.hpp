#pragma once

// vulkan base
#include "vvb_device.hpp"

class VvbBuffer
{
public:
	VvbBuffer(VvbDevice& vvbDevice, VkDeviceSize instanceSize, uint32_t instanceCount, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignement = 0);
	~VvbBuffer();

	// delete copy constructors
	VvbBuffer(const VvbBuffer&) = delete;
	VvbBuffer operator=(const VvbBuffer&) = delete;

	VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	void unmap();

	void write(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

	VkBuffer getBuffer() const { return buffer; }
	VkDeviceSize getSize() const { return bufferSize; }
	VkDescriptorBufferInfo getDescriptorBufferInfo() const { return VkDescriptorBufferInfo{ buffer, 0, VK_WHOLE_SIZE }; }

private:
	static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

	// vulkan base
	VvbDevice& vvbDevice;

	void* mapped = nullptr;
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;


	VkDeviceSize bufferSize;
	VkDeviceSize instanceSize;
	uint32_t instanceCount;

	VkDeviceSize alignmentSize;

	VkBufferUsageFlags usageFlags;
	VkMemoryPropertyFlags memoryPropertyFlags;
};