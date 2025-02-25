#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vector>
#include <optional>
#include <fstream>

//Project Files
#include "DeviceQuery.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"

class VulkanDevice
{
public:
    VulkanDevice();
    ~VulkanDevice();
    
    void RunApplication();
    
private:

    void InitWindow();
    void InitVulkan();
    void MainLoop();
    void CleanUp();
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void CreateSwapChain();
    void CreateImageViews();
    void CreateGraphicsPipeline();
    static std::vector<char> ReadFile(const std::string& fileName);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    void CreateRenderPass();
    void CreateFrameBuffers();
    void CreateCommandPool();
    void CreateCommandBuffer();
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void DrawFrame();
    void CreateSyncObjects();

    GLFWwindow* window = nullptr;

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    
    std::vector<VkFramebuffer> swapChainFrameBuffers;
    
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    DeviceQuery VdeviceQuery;
    VulkanInstance VInstance;
    VulkanSurface VSurface;
};