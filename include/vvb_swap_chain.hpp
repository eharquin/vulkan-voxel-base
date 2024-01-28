#pragma once

// vulkan base
#include "vvb_device.hpp"

// libs
#include <vulkan/vulkan_core.h>

// std
#include <vector>
#include <memory>

class VvbSwapChain
{
public:
	VvbSwapChain(VvbDevice& vvbDevice, VkExtent2D windowExtent);
	VvbSwapChain(VvbDevice& vvbDevice, VkExtent2D windowExtent, VkSwapchainKHR oldSwapChain);
	void init();

	~VvbSwapChain();


	// swap chain
	VkSwapchainKHR getSwapChain() { return swapChain; }
	VkExtent2D getExtent() { return swapChainExtent; }
	const float getAspectRatio() const { return swapChainExtent.width / (float) swapChainExtent.height; }
	uint32_t getCurrentFrame() { return currentFrame; }
	bool compareSwapFormat(const VvbSwapChain& swapChain) const
	{
		return swapChain.swapChainImageFormat == swapChainImageFormat;
	}

	// render pass
	VkRenderPass getRenderPass() { return renderPass; }

	// frame buffer
	VkFramebuffer getFramebuffer(int i) { return swapChainFramebuffers[i]; }

	// depth buffer


	// execution
	VkResult aquireNextImage(uint32_t* imageIndex);
	VkResult submitCommandBuffers(std::vector<VkCommandBuffer> commandBuffers, uint32_t* imageIndex);

private:
	// swap chain 
	VkExtent2D windowExtent;
	VkSwapchainKHR swapChain;
	VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	uint32_t currentFrame = 0;
	void createSwapChain();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	
	// image views
	std::vector<VkImageView> swapChainImageViews;
	void createImageViews();
	
	// render pass
	VkRenderPass renderPass;
	void createRenderPass();

	// frame buffer
	std::vector<VkFramebuffer> swapChainFramebuffers;
	void createFrameBuffers();

	// color image (Multisampling)
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
	void createColorResources();

	// depth buffer
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
	void createDepthResources();
	VkFormat findDepthFormat();

	// sync objects
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	void createSyncObjects();

	// vulkan base ref
	VvbDevice& vvbDevice;


};