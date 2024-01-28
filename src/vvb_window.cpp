// vulkan base
#include "vvb_window.hpp"

// std
#include <iostream>

VvbWindow::VvbWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name }
{
	initWindow();
}

VvbWindow::~VvbWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

int VvbWindow::getFramebufferSize(int* width, int* height)
{
	glfwGetFramebufferSize(window, width, height);
	
	if (*width == 0 || *height == 0)
		return 0;

	return 1;
}

void VvbWindow::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	
	/*
	glfwSetWindowPosCallback(window, windowPosCallback);
	glfwSetWindowRefreshCallback(window, windowRefreshCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);
	*/
}

void VvbWindow::framebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
	auto window = reinterpret_cast<VvbWindow*>(glfwGetWindowUserPointer(glfwWindow));
	window->framebufferResized = true;
	window->width = width;
	window->height = height;
}

void VvbWindow::windowPosCallback(GLFWwindow*, int xpos, int ypos)
{
	std::cout << "window pos callback" << std::endl;
}

void VvbWindow::windowRefreshCallback(GLFWwindow*)
{
	std::cout << "window refresh callback" << std::endl;
}

void VvbWindow::windowSizeCallback(GLFWwindow*, int width, int height)
{
	std::cout << "window size callback" << std::endl;
}