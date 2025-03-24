#pragma once

#include "VulkanIncludes.h"

//Project Files
#include "VulkanCore.h"

class VulkanDevice
{
public:
    VulkanDevice();
    ~VulkanDevice();
    
    void RunApplication();

    VulkanCore* GetVulkanCore();

private:
    void InitWindow();
    void InitVulkan();
    void MainLoop();
    void CleanUp() const;

    GLFWwindow* window = nullptr;

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    VulkanCore* VCore;
    
    CommandBuffer* VCommandBuffer;
    DeviceQuery* VDeviceQuery;
    VulkanInstance* VInstance;
    VulkanSurface* VSurface;
    SwapChain* VSwapChain;
    RenderPass* VRenderPass;
    Pipelines* VPipeline;
};