#pragma once

#include "VulkanIncludes.h"

#include <vector>

//Project Classes
class VulkanCore;

class VulkanSurface
{
public:
    VulkanSurface();
    ~VulkanSurface();
    
    void CreateSurface(VulkanCore* vulkanCore, GLFWwindow* window);

    VkSurfaceKHR& GetVulkanSurface();

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    
private:

    VkSurfaceKHR surface;
};
