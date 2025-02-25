#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanSurface
{
public:
    VulkanSurface();
    ~VulkanSurface();
    
    void CreateSurface(VkInstance instance, GLFWwindow* window);

    VkSurfaceKHR GetVulkanSurface();
    
private:

    VkSurfaceKHR surface;
};
