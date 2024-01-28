#pragma once
// vulkan base
#include "vvb_window.hpp"
#include "vvb_instance.hpp"
#include "vvb_device.hpp"
#include "vvb_renderer.hpp"
#include "vvb_descriptors.hpp"
#include "vvb_camera.hpp"

#include "keyboard_controller.hpp"

#include "system/voxel_render_system.hpp"

#include "model/voxel.hpp"


// libs
#include "GLFW/glfw3.h"

// std
#include <vector>

class App
{
public:

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	static constexpr int WIDTH = 1240;
	static constexpr int HEIGHT = 720;

	bool key = false;
	bool prevKey = false;

	App();
	~App();

	void run();

private:
	
	VvbWindow vvbWindow{ WIDTH, HEIGHT, "Vulkan Voxel window" };
	VvbInstance vvbInstance{};
	VvbDevice vvbDevice{ vvbWindow, vvbInstance };

	VvbRenderer vvbRenderer{ vvbWindow, vvbDevice };

	std::unique_ptr<VvbDescriptorPool> globalPool;

	void loadGameObject();
	void processInput(GLFWwindow* window);

	bool objectsRotation = false;
};
