#include "SwapChain.h"

#include <iostream>
#include <algorithm>

#include "DeviceQuery.h"
#include "VulkanSurface.h"
#include "RenderPass.h"
#include "VulkanCore.h"

SwapChain::SwapChain()
{
    swapChainImageFormat = VK_FORMAT_UNDEFINED;
    swapChainExtent = {0, 0};
    swapChainImages = {VK_NULL_HANDLE};
    swapChainImageViews = {VK_NULL_HANDLE};
    swapChain = VK_NULL_HANDLE;
    swapChainFrameBuffers = {VK_NULL_HANDLE};
}

SwapChain::~SwapChain()
{
}

/*
 * Vulkan does not have the concept of a "default framebuffer", hence it requires an infrastructure that will own the buffers we will render to before we visualize them on the screen. This infrastructure is known as Swap Chain.
 * and must be created explicitly by Vulkan.
 *
 * The Swap Chain is essentially a queue of images that are waiting to be presented to the screen. Our application will acquire such an image to draw to it, and then return it to the queue.
 *
 * How exactly the queue works and the conditions for presenting an image from the queue depend on how the swap chain is set up, but the general purpose of the swap chain is to synchronize the presentation of images with the refresh
 * rate of the screen.
 */
void SwapChain::CreateSwapChain(GLFWwindow* window)
{
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport();
    
    VkSurfaceFormatKHR surfaceFormat = VulkanCore::GetSurface()->ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window);

    //We include the +1 because it may take drivers time to complete internal operations
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainKHR oldSwapChain = swapChain;
    
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = VulkanCore::GetSurface()->GetVulkanSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    QueueFamilyIndices indices = VulkanCore::GetChosenDevice()->FindQueueFamilies();
    uint32_t queueFamilyIndices[]{ indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; //optional
        createInfo.pQueueFamilyIndices = nullptr; //optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapChain;
    
    if (vkCreateSwapchainKHR(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Create a Swap Chain!");
    }

    vkGetSwapchainImagesKHR(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    if (oldSwapChain != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(VulkanCore::GetChosenDevice()->GetChosenGPUDevice());
        vkDestroySwapchainKHR(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), oldSwapChain, nullptr);
    }
}

/*
 * The Frame buffers represent a collection of specific memory attachments that a Render Pass instance uses.
 *
 * A Framebuffer object references all the VkImageView objects that represent the attachments. In this case that will be our color attachment. However, the image that we have to use for the attachment depends on which image
 * the swap chain returns when we retrieve one for presentation. That means that we have to create a framebuffer for all the images in the swap chain and use the one that corresponds to the retrieved image at drawing time.
 */
void SwapChain::CreateFrameBuffers()
{
    swapChainFrameBuffers.resize(swapChainImageViews.size());
    for (size_t i = 0; i < swapChainFrameBuffers.size(); i++)
    {
        VkImageView attachments[] = { swapChainImageViews[i] };

        VkFramebufferCreateInfo frameBufferInfo{};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = VulkanCore::GetRenderPass()->GetVulkanRenderPass();
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = swapChainExtent.width;
        frameBufferInfo.height = swapChainExtent.height;
        frameBufferInfo.layers = 1;

        if (vkCreateFramebuffer(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &frameBufferInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create a Framebuffer!");
        }
    }
}

VkSwapchainKHR SwapChain::GetVulkanSwapChain()
{
	return swapChain;
}

SwapChainSupportDetails SwapChain::QuerySwapChainSupport()
{
    SwapChainSupportDetails details;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanCore::GetChosenDevice()->GetPhysicalDevice(), VulkanCore::GetSurface()->GetVulkanSurface(), &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanCore::GetChosenDevice()->GetPhysicalDevice(), VulkanCore::GetSurface()->GetVulkanSurface(), &formatCount, details.formats.data());

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanCore::GetChosenDevice()->GetPhysicalDevice(), VulkanCore::GetSurface()->GetVulkanSurface(), &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanCore::GetChosenDevice()->GetPhysicalDevice(), VulkanCore::GetSurface()->GetVulkanSurface(), &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanCore::GetChosenDevice()->GetPhysicalDevice(), VulkanCore::GetSurface()->GetVulkanSurface(), &presentModeCount, details.presentModes.data());
    }

    return details;
}

/*
 * An Image View is quite literally a view into an image. It describes how to access the image and which part of the image to access, for example if it should be treated as a 2D texture depth texture without any mipmapping levels.
 */
void SwapChain::CreateImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Image Views!");
        }
    }
}

void SwapChain::ReCreateSwapChain(GLFWwindow* window)
{
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }
    
    vkDeviceWaitIdle(VulkanCore::GetChosenDevice()->GetChosenGPUDevice());

    CleanUpSwapChain();
    
    CreateSwapChain(window);
    CreateImageViews();
    CreateFrameBuffers();
}

std::vector<VkFramebuffer>& SwapChain::GetSwapChainFrameBuffers()
{
    return swapChainFrameBuffers;
}

std::vector<VkImageView>& SwapChain::GetSwapChainImageViews()
{
    return swapChainImageViews;
}

std::vector<VkImage>& SwapChain::GetSwapChainImages()
{
    return swapChainImages;
}

VkFormat& SwapChain::GetSwapChainImageFormat()
{
    return swapChainImageFormat;
}

VkExtent2D& SwapChain::GetSwapChainImageExtent()
{
    return swapChainExtent;
}

void SwapChain::CleanUpSwapChain()
{
    for (VkFramebuffer& swapChainFrameBuffer : swapChainFrameBuffers)
    {
        vkDestroyFramebuffer(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), swapChainFrameBuffer, nullptr);
    }
    swapChainFrameBuffers.clear();

    for (VkImageView& swapChainImageView : swapChainImageViews)
    {
        vkDestroyImageView(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), swapChainImageView, nullptr);
    }
    swapChainImageViews.clear();

    if (swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), swapChain, nullptr);
        swapChain = VK_NULL_HANDLE;
    }
}

VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
    {
        return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}