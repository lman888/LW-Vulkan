#pragma once

#include "VulkanIncludes.h"

//Project Files
#include "VulkanCore.h"

class ModelLoader;

class VulkanDevice
{
public:
    VulkanDevice();
    ~VulkanDevice();
    
    void RunApplication();

    CommandBuffer* GetVulkanCommandBuffer();

private:
    void InitWindow();
    void InitVulkan() const;
    void MainLoop();
    void CleanUp() const;

    GLFWwindow* window = nullptr;

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    
    CommandBuffer* m_commandBuffer;
    DeviceQuery* m_deviceQuery;
    VulkanInstance* m_instance;
    VulkanSurface* m_surface;
    SwapChain* m_swapChain;
    RenderPass* m_renderPass;
    Pipelines* m_pipeline;
    Camera* m_camera;
    ModelLoader* m_castleModel;
    std::vector<ModelLoader>* m_models;
};
