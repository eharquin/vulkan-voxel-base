#pragma once

// vulkan base
#include "vvb_pipeline.hpp"
#include "vvb_mesh.hpp"
#include "model/voxel.hpp"

// libs
#include "GLFW/glfw3.h"

// std
#include <vector>
#include <memory>


class VoxelRenderSystem
{
public:
	VoxelRenderSystem(VvbDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayout);
	~VoxelRenderSystem();
	
	struct PushConstants
	{
		glm::mat4 data;
		glm::mat4 transform_matrix;
	};

	void update(glm::vec3 cameraPos, glm::vec3 cameraView);
	void render(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet);

private:
	// pipeline
	VkPipelineLayout pipelineLayout;
	std::unique_ptr<VvbPipeline> voxelPipeline;
	std::unique_ptr<VvbPipeline> outlinePipeline;

	// vulkan base ref
	VvbDevice& device;

	void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
	void createPipelines(VkRenderPass renderpass);

	World world{};
	std::unique_ptr<VvbMesh> chunk;
};

