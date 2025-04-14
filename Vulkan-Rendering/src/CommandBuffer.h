#pragma once

#include "VulkanIncludes.h"

#include "vector"

class DeviceQuery;
class VulkanSurface;
class RenderPass;
class SwapChain;
class Pipelines;

//We choose 2 so the CPU does not get too far ahead of the GPU.
//Do this better in future.
const int MAX_FRAMES_IN_FLIGHT = 2;

class CommandBuffer
{
public:
    CommandBuffer();
    ~CommandBuffer();
    

    void CreateCommandPool();
    void CreateCommandBuffers();
    
    void RecordCommandBuffer(uint32_t imageIndex);

    void CreateSyncObjects();

    void CleanUp() const;

    void DrawFrame(GLFWwindow* window);

    VkCommandBuffer BeginSingleTimeCommands();

    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

    VkCommandBuffer &GetCommandBuffer();
    VkCommandPool& GetCommandPool();

    void SetFrameBufferResized(bool resized);
    
private:
    
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    
    int currentFrame = 0;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    bool frameBufferResized = false;
};