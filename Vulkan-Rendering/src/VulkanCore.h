#pragma once

class Pipelines;
class CommandBuffer;
class RenderPass;
class SwapChain;
class VulkanSurface;
class DeviceQuery;
class VulkanInstance;

//This will hold pointers to all major Vulkan Objects
class VulkanCore
{
public:
    VulkanCore();
    ~VulkanCore();

    void SetDevice(DeviceQuery* device);
    DeviceQuery* GetChosenDevice() const;

    void SetInstance(VulkanInstance* instance);
    VulkanInstance* GetInstance() const;

    void SetSurface(VulkanSurface* surface);
    VulkanSurface* GetSurface() const;

    void SetSwapChain(SwapChain* swapChain);
    SwapChain* GetSwapChain() const;

    void SetRenderPass(RenderPass* renderPass);
    RenderPass* GetRenderPass() const;

    void SetCommandBuffer(CommandBuffer* commandBuffer);
    CommandBuffer* GetCommandBuffer();

    void SetPipeline(Pipelines* pipeline);
    Pipelines* GetPipeline();
    
private:

    DeviceQuery* VDevice;
    VulkanInstance* VInstance;
    VulkanSurface* VSurface;
    SwapChain* VSwapChain;
    RenderPass* VRenderPass;
    CommandBuffer* VCommandBuffer;
    Pipelines* VPipeline;
};