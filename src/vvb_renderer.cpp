// vulkan base
#include "vvb_renderer.hpp"

// std
#include <array>

VvbRenderer::VvbRenderer(VvbWindow& vvbWindow, VvbDevice& vvbDevice)
	: vvbWindow(vvbWindow), vvbDevice(vvbDevice)
{
	vvbSwapChain = std::make_unique<VvbSwapChain>(vvbDevice, vvbWindow.getExtent());
	allocateCommandBuffers();
}

VvbRenderer::~VvbRenderer()
{
	vvbSwapChain = nullptr;
}

VkCommandBuffer VvbRenderer::beginFrame()
{
	assert(!isFrameStarted && "can't call begin while already in progress!");

	VkResult result = vvbSwapChain->aquireNextImage(&currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return nullptr;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	
	isFrameStarted = true;

	VkCommandBuffer commandBuffer = getCurrentCommandBuffer();
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr; // optional
	beginInfo.flags = 0; // optional
	beginInfo.pInheritanceInfo = nullptr; // optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin recording command buffer!");

	return commandBuffer;
}

void VvbRenderer::endFrame()
{
	assert(isFrameStarted && "can't call endFrame while frame is not in progress!");
	VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to record command buffer!");

	VkResult result = vvbSwapChain->submitCommandBuffers(graphicsCommandBuffers, &currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vvbWindow.wasWindowResized())
	{
		vvbWindow.resetWindowResizedFlvvb();
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	isFrameStarted = false;
}

void VvbRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted && "can't call begin render pass if frame is not in progress!");
	assert(commandBuffer == getCurrentCommandBuffer() && "can't beging render pass on command buffer from a different frame");

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.35f, 0.56f, 0.72f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.pNext = nullptr; // optional
	renderPassInfo.renderPass = vvbSwapChain->getRenderPass();
	renderPassInfo.framebuffer = vvbSwapChain->getFramebuffer(currentImageIndex);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = vvbSwapChain->getExtent();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkExtent2D extent = vvbSwapChain->getExtent();

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VvbRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted && "can't end render pass if frame is not in progress!");
	assert(commandBuffer == getCurrentCommandBuffer() && "can't end render pass on command buffer from a different frame");

	vkCmdEndRenderPass(commandBuffer);
}

void VvbRenderer::recreateSwapChain()
{
	VkExtent2D extent = vvbWindow.getExtent();
	while (extent.width == 0 || extent.height == 0)
	{
		extent = vvbWindow.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(vvbDevice.getDevice());

	std::unique_ptr<VvbSwapChain> oldSwapChain = std::move(vvbSwapChain);
	vvbSwapChain = std::make_unique<VvbSwapChain>(vvbDevice, extent, oldSwapChain->getSwapChain());

	if (!oldSwapChain->compareSwapFormat(*vvbSwapChain))
		throw std::runtime_error("swap chain image format has changed!"); // TODO(enzo) : setup callback fonction notifying the app that a new incompatible render pass has been created

	oldSwapChain = nullptr;
}

void VvbRenderer::allocateCommandBuffers()
{
	// create graphics command buffers
	graphicsCommandBuffers.resize(vvbDevice.MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = nullptr; // optional
	allocInfo.commandPool = vvbDevice.getGraphicsCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(graphicsCommandBuffers.size());

	if (vkAllocateCommandBuffers(vvbDevice.getDevice(), &allocInfo, graphicsCommandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers!");
}