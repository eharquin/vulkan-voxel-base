#pragma once

// vulkan base
#include "vvb_swap_chain.hpp"
#include "vvb_device.hpp"

// libs

// std
#include <vector>
#include <memory>
#include <cassert>


class VvbRenderer
{
public:

	VvbRenderer(VvbWindow& vvbWindow, VvbDevice& vvbDevice);
	~VvbRenderer();

	VvbRenderer(const VvbRenderer&) = delete;
	VvbRenderer& operator=(const VvbRenderer&) = delete;

	// draw frame
	VkCommandBuffer beginFrame();
	void endFrame();

	// swapChain
	void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
	uint32_t getCurrentFrame() const { return vvbSwapChain->getCurrentFrame(); }
	VkExtent2D getSwapChainExtent() const { return vvbSwapChain->getExtent(); }
	const float getAspectRatio() const { return vvbSwapChain->getAspectRatio(); }

	VkRenderPass getRenderPass() const { return vvbSwapChain->getRenderPass(); }
	bool isFrameInProgress() const { return isFrameStarted; }
	VkCommandBuffer getCurrentCommandBuffer() const 
	{
		assert(isFrameStarted && "cannot get command buffer when frame is not in progress!");
		return graphicsCommandBuffers[vvbSwapChain->getCurrentFrame()];
	}

private:
	// vulkan base ref
	VvbWindow& vvbWindow;
	VvbDevice& vvbDevice;

	// swap chain
	std::unique_ptr<VvbSwapChain> vvbSwapChain;
	void recreateSwapChain();
	uint32_t currentImageIndex = 0;
	bool isFrameStarted = false;

	// command buffers
	std::vector<VkCommandBuffer> graphicsCommandBuffers;

	void allocateCommandBuffers();
};

