#include "VulkanInstance.h"

#include <iostream>
#include <vector>

VulkanInstance::VulkanInstance()
{
    instance = nullptr;
}

VulkanInstance::~VulkanInstance()
{
}

/* Instance Explanation 
 * The Instance is the connection between our Application and the Vulkan Library. (This is the very first thing we need to do in with Vulkan)
 * Creating this requires specifying details about our application to the Graphics Driver.
 *
 * The VkApplicationInfo Struct specifies if the application can support Vulkan 1.1 or later. As long as the application can support Vulkan 1.1,
 * it will create an Instance and Render things onto screen.
 *
 * First VkCreateInstance verifies that the requested "Layer" exists. If not, then VkCreateInstance will return a VK_ERROR_LAYER_NOT_PRESENT.
 * Second VkCreateInstance verifies that the requested extensions are supported (in the implemented or in any enabled instance layer), and if any requested extension
 * is not supported, VkCreateInstance must return VK_ERROR_EXTENSION_NOT_PRESENT.
 * After Verifying and enabling the instance layers and extensions, the VkInstance object is created and returned to the application.
 *
 * If a requested extension is only supported by a layer, both the layer and the extension need to be specified at VkCreateInstance time for the creation to succeed.
 *
 * See vkCreateInstance(3) Manual Page for additional information
 */
void VulkanInstance::CreateVulkanInstance()
{
    if (enableValidationLayers && CheckValidationLayerSupport() == false)
    {
        throw std::runtime_error("Validation layers requested, but non available");
    }
    
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Triangle Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    /* Check for Extension Support Here! */
    std::vector<const char*> extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }
    
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Instance");
    }
}

VkInstance& VulkanInstance::GetVulkanInstance()
{
    return instance;
}

bool VulkanInstance::GetEnableValidation() const
{
    return enableValidationLayers;
}

std::vector<const char*> VulkanInstance::GetValidationLayers() const
{
    return validationLayers;
}

void VulkanInstance::SetupDebugMessenger()
{
    if (enableValidationLayers == false)
    {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to set up Debug Messenger!");
    }
}

VkDebugUtilsMessengerEXT VulkanInstance::GetDebugMessenger()
{
    return debugMessenger;
}

/*
 * The Vulkan API os designed around the idea of minimal driver overhead and one of the main manifestations of that goal is that there is very limited error checking in the API by default.
 * Even small mistakes such as setting enumerations to incorrect values or passing null pointers to required parameters are generally not explicitly handled and will cause a crash or undefined behaviour.
 *
 * Vulkan requires us to be very explicit about everything we are doing, it is easy to make many small mistakes like using a new GPU feature and forgetting to request it at logical device creation time.
 *
 * However, this doesnt mean these safety checks cannot be added to the API. In Vulkan, they use a system called Validation layers. Validation Layers are optional components that hook into Vulkan function calls to apply additional operations.
 * Common Operations in the Validation layers are:
 * - Checking the values of parameters against the specification to detect misuse.
 * Tracking creation and destruction of objects to find resource leaks.
 * Checking thread safety by tracking the threads that calls originate from.
 * Logging every call and its parameters to the standard output.
 * Tracking Vulkan calls for profiling and replaying.
 *
 * The Validation Layer is included in the Vulkan SDK which we need to enable it by name, this is called VK_LAYER_KHRONOS_validation.
 * This is the main khronos validation layer. This layer enables us to verify our application is correctly using the Vulkan API.
 */
bool VulkanInstance::CheckValidationLayerSupport() const
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    bool layerFound = false;
    for (const char* layerName : validationLayers)
    {
        
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

    }
    
    return layerFound;
}

std::vector<const char*> VulkanInstance::GetRequiredExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extentions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
    {
        extentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extentions;
}

void VulkanInstance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
    createInfo.pUserData = nullptr; //optional
}

VkBool32 VulkanInstance::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "Validation Layer: " << pCallbackData->pMessage << "\n";
    
    return VK_FALSE;
}