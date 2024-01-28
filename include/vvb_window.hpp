#pragma once

// libs
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// std
#include <string>

class VvbWindow
{
public:
	VvbWindow(int w, int h, std::string name);
	~VvbWindow();

	// not copyable
	VvbWindow(const VvbWindow&) = delete;
	VvbWindow& operator=(const VvbWindow&) = delete;

	bool shouldClose() { return glfwWindowShouldClose(window); }
	VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
	GLFWwindow* getGLFWWindow() { return window; }
	bool wasWindowResized() { return framebufferResized; }
	void resetWindowResizedFlvvb() { framebufferResized = false; }

	int getFramebufferSize(int* width, int* height);

private:
	void initWindow();

	int width;
	int height;

	bool framebufferResized = false;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	
	static void windowPosCallback(GLFWwindow*, int xpos, int ypos);
	static void windowRefreshCallback(GLFWwindow*);
	static void windowSizeCallback(GLFWwindow*, int width, int height);



	std::string windowName;
	GLFWwindow* window;
};

