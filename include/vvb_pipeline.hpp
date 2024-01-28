#pragma once
// vulkan base
#include "vvb_swap_chain.hpp"

// std
#include <string>
#include <vector>

enum class Primitive
{
	TriangleList = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	LineList = VK_PRIMITIVE_TOPOLOGY_POINT_LIST
};

class VvbPipeline
{
public:
	VvbPipeline(VvbDevice& vvbDevice, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, const std::string& vertexFilepath, const std::string& fragmentFilepath, Primitive primitive = Primitive::TriangleList);
	~VvbPipeline();

	VkPipeline getVkPipeline() { return pipeline; }

	void bind(VkCommandBuffer commandBuffer);

private:
	VkPipeline pipeline;
	
	// vulkan base ref
	VvbDevice& vvbDevice;

	void createGraphicsPipeline(VkPipelineLayout pipelineLayout, VkRenderPass renderPass, const std::string& vertexFilepath, const std::string& fragmentFilepath, Primitive primitive);

	static std::vector<char> readFile(const std::string& filePath);
	VkShaderModule createShaderModule(const std::vector<char>& code);
};