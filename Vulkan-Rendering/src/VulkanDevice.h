#pragma once

#include "VulkanIncludes.h"

//Project Files
#include "DeviceQuery.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Pipelines.h"
#include "CommandBuffer.h"

class VulkanDevice
{
public:
    VulkanDevice();
    ~VulkanDevice();
    
    void RunApplication();

    CommandBuffer VCommandBuffer;
private:
    void InitWindow();
    void InitVulkan();
    void MainLoop();
    void CleanUp();

    GLFWwindow* window = nullptr;

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    DeviceQuery VDeviceQuery;
    VulkanInstance VInstance;
    VulkanSurface VSurface;
    SwapChain VSwapChain;
    RenderPass VRenderPass;
    Pipelines VPipeline;
};