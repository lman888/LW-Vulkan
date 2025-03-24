#include "VulkanCore.h"

VulkanCore::VulkanCore()
{
    VDevice = nullptr;
    VInstance = nullptr;
    VSurface = nullptr;
    VSwapChain = nullptr;
    VCommandBuffer = nullptr;
    VRenderPass = nullptr;
    VPipeline = nullptr;
}

VulkanCore::~VulkanCore()
{
    VDevice = nullptr;
    VInstance = nullptr;
    VSurface = nullptr;
    VSwapChain = nullptr;
    VCommandBuffer = nullptr;
    VRenderPass = nullptr;
    VPipeline = nullptr;
}

void VulkanCore::SetDevice(DeviceQuery* device)
{
    VDevice = device;
}

DeviceQuery* VulkanCore::GetChosenDevice() const
{
    return VDevice;
}

void VulkanCore::SetInstance(VulkanInstance* instance)
{
    VInstance = instance;
}

VulkanInstance* VulkanCore::GetInstance() const
{
    return VInstance;
}

void VulkanCore::SetSurface(VulkanSurface* surface)
{
    VSurface = surface;
}

VulkanSurface* VulkanCore::GetSurface() const
{
    return VSurface;
}

void VulkanCore::SetSwapChain(SwapChain* swapChain)
{
    VSwapChain = swapChain;
}

SwapChain* VulkanCore::GetSwapChain() const
{
    return VSwapChain;
}

void VulkanCore::SetRenderPass(RenderPass* renderPass)
{
    VRenderPass = renderPass;
}

RenderPass* VulkanCore::GetRenderPass() const
{
    return VRenderPass;
}

void VulkanCore::SetCommandBuffer(CommandBuffer* commandBuffer)
{
    VCommandBuffer = commandBuffer;
}

CommandBuffer* VulkanCore::GetCommandBuffer()
{
    return VCommandBuffer;
}

void VulkanCore::SetPipeline(Pipelines* pipeline)
{
    VPipeline = pipeline;
}

Pipelines* VulkanCore::GetPipeline()
{
    return VPipeline;
}