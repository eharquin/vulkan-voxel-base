#pragma once
// vulkan base
#include "vvb_device.hpp"
#include "vvb_buffer.hpp"

//std
#include <unordered_map>

// ********************* Descriptor Pool *********************

class VvbDescriptorPool
{
public:
	VvbDescriptorPool(VvbDevice& vvbDevice, uint32_t poolSizeCount, VkDescriptorPoolSize* poolSizes, VkDescriptorPoolCreateFlags flags = 0, uint32_t maxSets = 1000);
	~VvbDescriptorPool();

	VkDescriptorPool getPool() const { return pool; }

	bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

private:
	// vulkan base ref
	VvbDevice& vvbDevice;

	VkDescriptorPool pool = VK_NULL_HANDLE;

};

// ****************** Descriptor Set Layout ******************

class VvbDescriptorSetLayout
{
public:
	VvbDescriptorSetLayout(VvbDevice& vvbDevice, uint32_t bindingCount, VkDescriptorSetLayoutBinding* bindings, VkDescriptorSetLayoutCreateFlags flags = 0);
	~VvbDescriptorSetLayout();

	VkDescriptorSetLayout getLayout() const { return descriptorSetLayout; }

private:
	// vulkan base ref
	VvbDevice& vvbDevice;
	
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindingsMap{};

	friend class VvbDescriptorSetWriter;
};

// ****************** Descriptor Set Writer ******************

class VvbDescriptorSetWriter
{
public:
	VvbDescriptorSetWriter(VvbDescriptorSetLayout& layout);
	~VvbDescriptorSetWriter();
	
	void writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
	void writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);
	void writeTexel();

	void overwrite(VkDescriptorSet& descriptorSet);

private:
	// vulkan base ref
	VvbDescriptorSetLayout& layout;

	std::vector<VkWriteDescriptorSet> writes{};
};
