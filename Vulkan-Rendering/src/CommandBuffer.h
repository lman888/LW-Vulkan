#pragma once

#include "VulkanIncludes.h"

#include "vector"

class DeviceQuery;
class VulkanSurface;
class RenderPass;
class SwapChain;
class Pipelines;

class CommandBuffer
{
public:
    CommandBuffer();
    ~CommandBuffer();
    

    void CreateCommandPool(DeviceQuery& device, VulkanSurface& surface);
    void CreateCommandBuffers(DeviceQuery& device);
    
    void RecordCommandBuffer(uint32_t imageIndex, RenderPass& renderPass, SwapChain& swapChain, Pipelines& pipeline);

    void CreateSyncObjects(DeviceQuery& device);

    void CleanUp(DeviceQuery& device) const;

    void DrawFrame(DeviceQuery& device, SwapChain& swapChain, Pipelines& pipeline, RenderPass& renderPass, VulkanSurface& surface, GLFWwindow* window);

    VkCommandBuffer &GetCommandBuffer();
    VkCommandPool& GetCommandPool();

    void SetFrameBufferResized(bool resized);
    
private:
    
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    //We choose 2 so the CPU does not get too far ahead of the GPU.
    const int MAX_FRAMES_IN_FLIGHT = 2;
    
    int currentFrame = 0;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    bool frameBufferResized = false;
};