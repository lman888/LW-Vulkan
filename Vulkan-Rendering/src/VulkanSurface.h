#pragma once

#include "VulkanIncludes.h"

#include <vector>

class VulkanSurface
{
public:
    VulkanSurface();
    ~VulkanSurface();
    
    void CreateSurface(GLFWwindow* window);

    VkSurfaceKHR& GetVulkanSurface();

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    
private:

    VkSurfaceKHR surface;
};
