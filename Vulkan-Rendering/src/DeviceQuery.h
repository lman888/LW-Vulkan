#pragma once

#include "VulkanIncludes.h"

#include <optional>
#include <vector>

class VulkanSurface;
class VulkanCore;
class SwapChain;

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};


/* 
 * The purpose of this class is to query the best GPU based on what functionality it supports with Vulkan and provide it to the Main Vulkan Application
 */
class DeviceQuery
{
public:
    DeviceQuery();
    ~DeviceQuery();

    void PickPhysicalDevice();
    void CreateLogicalDevice();
    
    VkDevice& GetChosenGPUDevice();
    VkPhysicalDevice& GetPhysicalDevice();
    VkQueue& GetGraphicsQueue();
    VkQueue& GetPresentQueue();

    QueueFamilyIndices FindQueueFamilies() const;

private:

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkDevice chosenDevice;
    VkPhysicalDevice physicalDevice;
    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    bool IsDeviceSuitable(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
};
