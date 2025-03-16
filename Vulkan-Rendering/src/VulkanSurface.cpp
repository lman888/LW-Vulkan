#include "VulkanSurface.h"

#include <iostream>
#include <vector>

VulkanSurface::VulkanSurface()
{
    surface = nullptr;
}

VulkanSurface::~VulkanSurface()
{
}

VkSurfaceKHR& VulkanSurface::GetVulkanSurface()
{
    return surface;
}

/*
 * Since the Vulkan platform is an agnostic API, it can not directly interface with the window system on its own. To establish the connection between Vulkan and the window system to present results to the screen,
 * we need to use the WSI (Window System Integration) extensions.
 *
 * The VK_KHR_surface extension is an Instance level extension and we've already enabled it, this was included in the list returned by glfwGetRequiredInstanceExtensions.
 *
 * The Window Surface needs to be created right after the instance creation, because it can actually influence the physical device selection. 
 */
void VulkanSurface::CreateSurface(VkInstance& instance, GLFWwindow* window)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Window Surface!");
    }
}

VkSurfaceFormatKHR VulkanSurface::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}