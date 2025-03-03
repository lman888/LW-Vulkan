#pragma once

#include "VulkanIncludes.h"

#include <vector>
#include <optional>
#include <fstream>

//Project Files
#include "DeviceQuery.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "SwapChain.h"

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
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
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
    
    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    DeviceQuery VDeviceQuery;
    VulkanInstance VInstance;
    VulkanSurface VSurface;
    SwapChain VSwapChain;
};