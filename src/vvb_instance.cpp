#include "vvb_instance.hpp"

// libs
#include <GLFW/glfw3.h>

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>

VvbInstance::VvbInstance()
{
    createInstance();

    if (enableValidationLayers)
        setupDebugMessenger();
}

VvbInstance::~VvbInstance()
{
    if (enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    vkDestroyInstance(instance, nullptr);
}

void VvbInstance::createInstance()
{
    if (enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("validation layers requested, but not available!");

    // define application Info, Name, Version
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "AE graphics app";
    appInfo.pNext = nullptr; // optional
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "AstroEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // get layers to enable
    uint32_t layerCount = 0;
    std::vector<const char*> layerNames;
    if (enableValidationLayers)
    {
        layerCount = static_cast<uint32_t>(validationLayers.size());
        layerNames = validationLayers;
    }

    // get required extensions
    std::vector<const char*> requiredExtensions = getRequiredExtensions();

    // get available instance extensions
    std::vector<VkExtensionProperties> extensions = getInstanceExtensions();

    // get DebugMessenger info
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers)
    {
        populateDebugMessengerCreateInfo(debugCreateInfo);
    }

    // define instance creation info 
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = &debugCreateInfo;
    createInfo.flags = 0; // optional
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = layerCount;
    createInfo.ppEnabledLayerNames = layerNames.data();

#ifdef __APPLE__
    requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");

    // print extensions information
    if (printInfo)
    {
        std::cout << "glfw uses extensions:\n";
        for (auto const& extension : requiredExtensions)
        {
            std::cout << '\t' << extension << std::endl;
        }

        std::cout << "available instance extensions:\n";
        for (auto const& extension : extensions)
        {
            std::cout << '\t' << extension.extensionName << " " << extension.specVersion << std::endl;
        }
    }
}

void VvbInstance::setupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        throw std::runtime_error("failed to set up debug messenger!");
}

std::vector<const char*> VvbInstance::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensionsNames = nullptr;
    glfwExtensionsNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // initialize vector using input iterator
    std::vector<const char*> extensions(glfwExtensionsNames, glfwExtensionsNames + glfwExtensionCount);
    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

std::vector<VkExtensionProperties> VvbInstance::getInstanceExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    return extensions;
}

bool VvbInstance::checkValidationLayerSupport()
{
    // get instance layers
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // check if the defined layers exist
    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers)
        {
            if (std::strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}

void VvbInstance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.pNext = nullptr; // optional
    createInfo.flags = 0; // optional
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // optional

    //VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT

}
