// vulkan base
#include "app.hpp"

// std
#include <iostream>
#include <memory>
#include <chrono>

App::App()
{
	// create global descriptor pool
	VkDescriptorPoolSize uboPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, vvbDevice.MAX_FRAMES_IN_FLIGHT };
	VkDescriptorPoolSize samplerPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, vvbDevice.MAX_FRAMES_IN_FLIGHT };
	std::array<VkDescriptorPoolSize, 2> poolSizes{ uboPoolSize , samplerPoolSize };

	globalPool = std::make_unique<VvbDescriptorPool>(vvbDevice, poolSizes.size(), poolSizes.data(), 0, vvbDevice.MAX_FRAMES_IN_FLIGHT);

	loadGameObject();
}

App::~App() {}


void App::run()
{
	
	// create ubo buffers
	std::vector<std::unique_ptr<VvbBuffer>> uboBuffers{ vvbDevice.MAX_FRAMES_IN_FLIGHT };
	for (int i = 0; i < uboBuffers.size(); i++)
	{
		uboBuffers[i] = std::make_unique<VvbBuffer>(
			vvbDevice,
			sizeof(UniformBufferObject),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		uboBuffers[i]->map();
	}

	VkDescriptorSetLayoutBinding uboBinding{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 , VK_SHADER_STAGE_VERTEX_BIT, nullptr };
	VkDescriptorSetLayoutBinding samplerBinding{ 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 , VK_SHADER_STAGE_FRAGMENT_BIT, nullptr };
	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboBinding, samplerBinding };

	VvbDescriptorSetLayout globalSetLayout(vvbDevice, bindings.size(), bindings.data());

	std::vector<VkDescriptorSetLayout> layouts(vvbDevice.MAX_FRAMES_IN_FLIGHT, globalSetLayout.getLayout());
	std::vector<VkDescriptorSet> globalDescriptorsSets(vvbDevice.MAX_FRAMES_IN_FLIGHT);



	for (int i = 0; i < vvbDevice.MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo = uboBuffers[i]->getDescriptorBufferInfo();

		// Texture link
		//VkDescriptorImageInfo imageInfo{};
		//imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		//imageInfo.imageView = gameObjects.at(0).model->getTextureImageView(); // Error
		//imageInfo.sampler = gameObjects.at(0).model->getTextureSampler(); // Error

		globalPool->allocateDescriptor(globalSetLayout.getLayout(), globalDescriptorsSets[i]);
		VvbDescriptorSetWriter writer{ globalSetLayout };
		writer.writeBuffer(0, &bufferInfo);
		//writer.writeImage(1, &imageInfo);
		writer.overwrite(globalDescriptorsSets[i]);
	}

	// tempo solution
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraRot = glm::vec3(0.0f, 0.0f, 0.0f);

	VoxelRenderSystem renderSystem = VoxelRenderSystem(vvbDevice, vvbRenderer.getRenderPass(), globalSetLayout.getLayout());
	renderSystem.update(cameraPos, cameraRot);

	VvbCamera camera;
	KeyboardController keyboardController;

	auto currentTime = std::chrono::high_resolution_clock::now();

	while (!vvbWindow.shouldClose())
	{
		glfwPollEvents();

		processInput(vvbWindow.getGLFWWindow());

		auto newTime = std::chrono::high_resolution_clock::now();

		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - newTime).count();

		currentTime = newTime;
		
		// set camera view
		//camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));
		//camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));
		keyboardController.moveInPlaneXZ(vvbWindow.getGLFWWindow(), time, cameraPos, cameraRot);
		camera.setViewYXZ(cameraPos, cameraRot);

		// set camera projection
		float aspect = vvbRenderer.getAspectRatio();
		//camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
		camera.setPerspectiveProjection(glm::radians(50.0f), vvbRenderer.getAspectRatio(), 0.1f, 10.0f);

		if (VkCommandBuffer commandBuffer = vvbRenderer.beginFrame())
		{
			int frameIndex = vvbRenderer.getCurrentFrame();

			// update
			UniformBufferObject ubo{};
			ubo.model = glm::mat4(1.0f);
			ubo.view = camera.getView();
			ubo.proj = camera.getProjection();
			uboBuffers[frameIndex]->write(&ubo);
			//renderSystem.update(cameraPos, cameraRot);

			// render
			vvbRenderer.beginSwapChainRenderPass(commandBuffer);
			renderSystem.render(commandBuffer, globalDescriptorsSets[frameIndex]);
			vvbRenderer.endSwapChainRenderPass(commandBuffer);
			vvbRenderer.endFrame();
		}
	}
	vkDeviceWaitIdle(vvbDevice.getDevice());
}

void App::loadGameObject()
{
	//std::shared_ptr<VvbModel> model = testCubeFace();
	//auto cube = VvbGameObject::createGameObject();
	//cube.model = model;
	//cube.transform.translation = { .0f, .0f, 2.5f };
	//cube.transform.scale = { .5f, .5f, .5f };
	//gameObjects.push_back(std::move(cube));
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void App::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	prevKey = key;
	key = (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS);

	if (key && !prevKey)
	{
		objectsRotation = !objectsRotation;
	}
}

