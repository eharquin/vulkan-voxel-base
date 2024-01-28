// vulkan base
#include "system/voxel_render_system.hpp"

// std
#include <memory>

VoxelRenderSystem::VoxelRenderSystem(VvbDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout)
	: device(device)
{
	chunk = std::make_unique<VvbMesh>(device, world.chunks[0]);
	createPipelineLayout(descriptorSetLayout);
	createPipelines(renderPass);
}

VoxelRenderSystem::~VoxelRenderSystem()
{
	vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
}

void VoxelRenderSystem::update(glm::vec3 cameraPos, glm::vec3 cameraView)
{
	world.update(.1f, cameraPos, cameraView);
}

void VoxelRenderSystem::render(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet)
{
	for (int chunkIndex = 0; chunkIndex < world.renderList.size(); chunkIndex++)
	{
		glm::vec3 chunkPosition(chunkIndex % World::WorldSize,
							   (chunkIndex / World::WorldSize) % World::WorldSize,
							   (chunkIndex / (World::WorldSize * World::WorldSize)) % World::WorldSize);

		PushConstants pushConstants{};
		pushConstants.data = glm::mat4(1.0f);
		pushConstants.transform_matrix = glm::translate(glm::mat4(1.0f), chunkPosition * glm::vec3(Chunk::ChunkSize));

		voxelPipeline->bind(commandBuffer);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstants);

		chunk->bind(commandBuffer);
		chunk->draw(commandBuffer);

		outlinePipeline->bind(commandBuffer);
		chunk->bind(commandBuffer);
		chunk->draw(commandBuffer);
	}
}

 void VoxelRenderSystem::createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout)
{
	// define push constants
	VkPushConstantRange pushConstantsInfo{};
	pushConstantsInfo.offset = 0;
	pushConstantsInfo.size = sizeof(PushConstants);
	// push constants onlly accesible from vertex shaders
	pushConstantsInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	// create pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = nullptr; // optional
	pipelineLayoutInfo.flags = 0; // optional
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantsInfo;

	if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create pipeline layout!");
}

void VoxelRenderSystem::createPipelines(VkRenderPass renderPass)
{
	voxelPipeline = std::make_unique<VvbPipeline>(device, pipelineLayout, renderPass, "shaders/voxel.vert.spv", "shaders/voxel.frag.spv");
	outlinePipeline = std::make_unique<VvbPipeline>(device, pipelineLayout, renderPass, "shaders/outline.vert.spv", "shaders/outline.frag.spv", Primitive::LineList);
}
