#pragma once

#include "VulkanIncludes.h"

#include <vector>
#include <optional>

//Project Files
#include "DeviceQuery.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Pipelines.h"

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
    
    void CreateFrameBuffers();
    void CreateCommandPool();
    void CreateCommandBuffer();
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void DrawFrame();
    void CreateSyncObjects();

    GLFWwindow* window = nullptr;

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    DeviceQuery VDeviceQuery;
    VulkanInstance VInstance;
    VulkanSurface VSurface;
    SwapChain VSwapChain;
    RenderPass VRenderPass;
    Pipelines VPipeline;
};