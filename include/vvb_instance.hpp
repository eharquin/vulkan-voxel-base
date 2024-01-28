#pragma once

// libs
#include <vulkan/vulkan.h>

// std
#include <vector>
#include <iostream>

#ifdef NDEBUG
	const bool printInfo = true;
	const bool enableValidationLayers = false;
#else
	const bool printInfo = true;
	const bool enableValidationLayers = true;
#endif // DEBUG

class VvbInstance
{
public:
	
	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

	VvbInstance();
	~VvbInstance();

	VkInstance getInstance() { return instance; }


private:
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	
	void createInstance();
	void setupDebugMessenger();

	std::vector<const char*> getRequiredExtensions();

	std::vector<VkExtensionProperties> getInstanceExtensions();

	bool checkValidationLayerSupport();

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(instance, debugMessenger, pAllocator);
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::cerr << "validation layer: " << messageSeverity << " " << messageType << " " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};