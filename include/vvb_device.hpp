#pragma once
// vulkan base
#include "vvb_window.hpp"
#include "vvb_instance.hpp"

// libs
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vulkan/vulkan_core.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// std
#include <optional>

class VvbDevice
{
public:
	VvbDevice(VvbWindow& vvbWindow, VvbInstance& vvbInstance);
	~VvbDevice();

	const unsigned int MAX_FRAMES_IN_FLIGHT = 2;

	// surface
	VkSurfaceKHR getSurface() { return surface; }

	// physical device
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> transferFamily;
		bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value(); }
		bool isGraphicsTransferQueueConcurent() { return graphicsFamily.value() != transferFamily.value(); }
	};
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device); // TODO : check when it should be reask
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
	VkPhysicalDeviceProperties getPhysicalDeviceProperties() const { return physicalDeviceProperties; }
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkSampleCountFlagBits getMsaaSamplesCount() const { return msaaSamples; }

	// device
	VkDevice getDevice() { return device; }
	VkQueue getGraphicsQueue() { return graphicsQueue; }
	VkQueue getPresentQueue() { return presentQueue; }
	VkQueue getTransferQueue() { return transferQueue; }
	QueueFamilyIndices getQueueFamilyIndices() { return indices; }

	// command pools
	VkCommandPool getGraphicsCommandPool() { return graphicsCommandPool; }
	VkCommandPool getTransferCommandPool() { return transferCommandPool; }

	// buffer utils
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	// image utils
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t mipLevels, VkSampleCountFlagBits numSamples);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	bool hasStencilComponent(VkFormat format);
	void generateMipmaps(VkImage image, VkFormat imageFormat, uint32_t width, uint32_t height, uint32_t mipLevels);

private:
	// vulkan base ref
	VvbWindow& vvbWindow;
	VvbInstance& vvbInstance;

	// surface
	VkSurfaceKHR surface;
	void createSurface();

	// physical device
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties physicalDeviceProperties{};
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	void pickPhysicalDevice();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	int rateDeviceSuitability(VkPhysicalDevice device);
	VkSampleCountFlagBits getMaxUsableSampleCount();
	std::vector<VkPhysicalDevice> getPhysicalDevices();
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device); // TODO : verify if it's important to reask SPOILER ALERT YESSSS but WHEN??

	// device
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue transferQueue;
	QueueFamilyIndices indices;
	void createDevice();

	// command pools
	VkCommandPool graphicsCommandPool;
	VkCommandPool transferCommandPool;
	void createCommandPools();

	// TODO : take in count the poolCreator of the command
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
};

