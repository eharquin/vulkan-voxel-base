// vulkan base
#include "vvb_pipeline.hpp"
#include "vvb_model.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <array>



VvbPipeline::VvbPipeline(VvbDevice& vvbDevice, VkPipelineLayout pipelineLayout, VkRenderPass renderPass, const std::string& vertexFilepath, const std::string& fragmentFilepath, Primitive primitive)
	: vvbDevice(vvbDevice)
{
	createGraphicsPipeline(pipelineLayout, renderPass, vertexFilepath, fragmentFilepath, primitive);
}

VvbPipeline::~VvbPipeline()
{
	std::cout << "Destroy pipeline" << std::endl;
	vkDestroyPipeline(vvbDevice.getDevice(), pipeline, nullptr);
}

void VvbPipeline::bind(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void VvbPipeline::createGraphicsPipeline(VkPipelineLayout pipelineLayout, VkRenderPass renderPass, const std::string& vertexFilepath, const std::string& fragmentFilepath, Primitive primitive)
{

	std::vector<char> vertexShaderCode = readFile(vertexFilepath);
	VkShaderModule vertexShader  = createShaderModule(vertexShaderCode);

	std::vector<char> fragmentShaderCode = readFile(fragmentFilepath);
	VkShaderModule fragmentShader = createShaderModule(fragmentShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.pNext = nullptr; // optional
	vertShaderStageInfo.flags = 0; // optional
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertexShader;
	vertShaderStageInfo.pName = "main";
	vertShaderStageInfo.pSpecializationInfo = nullptr; // optional

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.pNext = nullptr; // optional
	vertShaderStageInfo.flags = 0; // optional
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragmentShader;
	fragShaderStageInfo.pName = "main";
	vertShaderStageInfo.pSpecializationInfo = nullptr; // optional

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH };

	auto bindingDescription = VvbModel::Vertex::getBindingDescription();
	auto attributeDescriptions = VvbModel::Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pNext = nullptr; // optional
	vertexInputInfo.flags = 0; // optional
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.pNext = nullptr; // optional
	inputAssembly.flags = 0; // optional
	inputAssembly.topology = (VkPrimitiveTopology) primitive;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = nullptr; // optional
	viewportState.flags = 0; // optional
	viewportState.viewportCount = 1;
	viewportState.pViewports = nullptr;
	viewportState.scissorCount = 1;
	viewportState.pScissors = nullptr;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.pNext = nullptr; // optional
	rasterizer.flags = 0; // optional
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // optional
	rasterizer.depthBiasClamp = 0.0f; // optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // optional
	rasterizer.lineWidth = 1.0f;
	
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.pNext = nullptr; // optional
	multisampling.flags = 0; // optional
	multisampling.rasterizationSamples = vvbDevice.getMsaaSamplesCount();
	multisampling.sampleShadingEnable = VK_TRUE;
	multisampling.minSampleShading = .2f; // min fraction for sample shading; closer to one is smooth
	multisampling.pSampleMask = nullptr; // optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // optional
	multisampling.alphaToOneEnable = VK_FALSE; // optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // optional
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.pNext = nullptr; // optional
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext = nullptr; // optional
	colorBlending.flags = 0; // optional
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // optional
	colorBlending.blendConstants[1] = 0.0f; // optional
	colorBlending.blendConstants[2] = 0.0f; // optional
	colorBlending.blendConstants[3] = 0.0f; // optional

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pNext = nullptr; // optional
	dynamicState.flags = 0; // optional
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr; // optional
	pipelineInfo.flags = 0; // optional
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pTessellationState = nullptr; // optional
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // optional
	pipelineInfo.basePipelineIndex = -1; // optional

	if (vkCreateGraphicsPipelines(vvbDevice.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
		throw std::runtime_error("failed to create graphics pipeline!");
	
	// shader are deleted here
	vkDestroyShaderModule(vvbDevice.getDevice(), vertexShader, nullptr);
	vkDestroyShaderModule(vvbDevice.getDevice(), fragmentShader, nullptr);
}


std::vector<char> VvbPipeline::readFile(const std::string& filePath)
{
	// construct an ifstream object and open a file with the cursor at the end (ate) and in binary reader mode
	std::ifstream file{ filePath, std::ios::ate | std::ios::binary };

	if (!file.is_open())
		throw std::runtime_error{ "failed to open file: " + filePath };

	// get file size by using the cursor at the end
	size_t fileSize = static_cast<size_t>(file.tellg());

	std::vector<char> buffer(fileSize);

	// seek back the cursor to the beginning
	file.seekg(0);

	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

VkShaderModule VvbPipeline::createShaderModule(const std::vector<char>& code)
{
	VkShaderModule shader;

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr; // optional
	createInfo.flags = 0; // optional
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(vvbDevice.getDevice(), &createInfo, nullptr, &shader) != VK_SUCCESS)
		throw std::runtime_error("failed to create shader module!");

	return shader;
}
