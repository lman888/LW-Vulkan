#pragma once

#include "VulkanIncludes.h"

#include <optional>

using std::vector;
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

    void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, SwapChain swapChain);
    void CreateLogicalDevice(VkSurfaceKHR surface, bool enableValidationLayers, std::vector<const char*> validationLayers);
    
    VkDevice GetChosenDevice() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    VkQueue GetGraphicsQueue() const;
    VkQueue GetPresentQueue() const;

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

private:

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkDevice chosenDevice;
    VkPhysicalDevice physicalDevice;
    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, SwapChain swapChain);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
};
