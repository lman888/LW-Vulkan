#include "DeviceQuery.h"
#include <iostream>
#include <vector>
#include "SwapChain.h"
#include "set"
#include "VulkanCore.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"

DeviceQuery::DeviceQuery()
{
    chosenDevice = VkDevice();
    physicalDevice = VK_NULL_HANDLE;
}

DeviceQuery::~DeviceQuery()
{
}

/*
 * The Physical Device usually represents a single complete implementation of Vulkan (excluding instance-level functionality) available to the host, of which there are a finite number. (In other words, the Physical Device is our GPU)
 *
 * The GPU we end up selecting will be stored in a VkPhysical Device variable. We select a Physical Device based on the operations we wish to perform in Vulkan because not all GPUs are made equal.
 *
 * To decide which device to use, we start by querying for details. Basic device properties like the name, type and supported Vulkan version. (This is queried using vkGetPhysicalDeviceProperties)
 *
 * Another aspect of finding a suitable device is Queue Families. Queue families each have their own subset of commands. Example, there could be a queue family that only allows processing of compute commands, or one that allows
 * memory transfer related commands.
 */
void DeviceQuery::PickPhysicalDevice(VulkanCore* vulkanCore)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vulkanCore->GetInstance()->GetVulkanInstance(), &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vulkanCore->GetInstance()->GetVulkanInstance(), &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device, vulkanCore))
        {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find suitable GPU!");
    }
}

/*
 * The Logical device creation process is similar to the instance creation process and describes the features we want to use. 
 *
 * A Logical device represents an instance of the implementation with its own state and resources independant of other logical devices.
 */
void DeviceQuery::CreateLogicalDevice(VulkanCore* vulkanCore)
{
    QueueFamilyIndices indices = FindQueueFamilies(vulkanCore);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &chosenDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Logical Device!");
    }
    
    vkGetDeviceQueue(chosenDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(chosenDevice, indices.presentFamily.value(), 0, &presentQueue);
}

VkPhysicalDevice& DeviceQuery::GetPhysicalDevice()
{
    return physicalDevice;
}

VkQueue& DeviceQuery::GetGraphicsQueue()
{
    return graphicsQueue;
}

VkQueue& DeviceQuery::GetPresentQueue()
{
    return presentQueue;
}

VkDevice& DeviceQuery::GetChosenGPUDevice()
{
    return chosenDevice;
}

QueueFamilyIndices DeviceQuery::FindQueueFamilies(VulkanCore* vulkanCore)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, vulkanCore->GetSurface()->GetVulkanSurface(), &presentSupport);

        if (presentSupport)
        {
            indices.presentFamily = i;
        }
        
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }
        
        if (indices.IsComplete())
        {
            break;
        }
        
        i++;
    }
    
    return indices;
}

bool DeviceQuery::IsDeviceSuitable(VkPhysicalDevice device, VulkanCore* vulkanCore)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    vulkanCore->GetChosenDevice()->physicalDevice = device;
    QueueFamilyIndices indices = FindQueueFamilies(vulkanCore);

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = vulkanCore->GetSwapChain()->QuerySwapChainSupport(vulkanCore);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    
    return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

bool DeviceQuery::CheckDeviceExtensionSupport(VkPhysicalDevice device) const
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }
    
    return requiredExtensions.empty();
}