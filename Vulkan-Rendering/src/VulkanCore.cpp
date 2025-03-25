#include "VulkanCore.h"

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
    Get().ISetDevice(device);
}

DeviceQuery* VulkanCore::GetChosenDevice()
{
    return Get().IGetChosenDevice();
}

void VulkanCore::SetInstance(VulkanInstance* instance)
{
    Get().ISetInstance(instance);
}

VulkanInstance* VulkanCore::GetInstance()
{
    return Get().IGetInstance();
}

void VulkanCore::SetSurface(VulkanSurface* surface)
{
    Get().ISetSurface(surface);
}

VulkanSurface* VulkanCore::GetSurface()
{
    return Get().IGetSurface();
}

void VulkanCore::SetSwapChain(SwapChain* swapChain)
{
    Get().ISetSwapChain(swapChain);
}

SwapChain* VulkanCore::GetSwapChain()
{
    return Get().IGetSwapChain();
}

void VulkanCore::SetRenderPass(RenderPass* renderPass)
{
    Get().ISetRenderPass(renderPass);
}

RenderPass* VulkanCore::GetRenderPass()
{
    return Get().IGetRenderPass();
}

void VulkanCore::SetCommandBuffer(CommandBuffer* commandBuffer)
{
    Get().ISetCommandBuffer(commandBuffer);
}

CommandBuffer* VulkanCore::GetCommandBuffer()
{
    return Get().IGetCommandBuffer();
}

void VulkanCore::SetPipeline(Pipelines* pipeline)
{
    Get().ISetPipeline(pipeline);
}

Pipelines* VulkanCore::GetPipeline()
{
    return Get().IGetPipeline();
}

void VulkanCore::ISetDevice(DeviceQuery* device)
{
    VDevice = device;
}

DeviceQuery* VulkanCore::IGetChosenDevice() const
{
    return VDevice;
}

void VulkanCore::ISetInstance(VulkanInstance* instance)
{
    VInstance = instance;
}

VulkanInstance* VulkanCore::IGetInstance() const
{
    return VInstance;
}

void VulkanCore::ISetSurface(VulkanSurface* surface)
{
    VSurface = surface;
}

VulkanSurface* VulkanCore::IGetSurface() const
{
    return VSurface;
}

void VulkanCore::ISetSwapChain(SwapChain* swapChain)
{
    VSwapChain = swapChain;
}

SwapChain* VulkanCore::IGetSwapChain() const
{
    return VSwapChain;
}

void VulkanCore::ISetRenderPass(RenderPass* renderPass)
{
    VRenderPass = renderPass;
}

RenderPass* VulkanCore::IGetRenderPass() const
{
    return VRenderPass;
}

void VulkanCore::ISetCommandBuffer(CommandBuffer* commandBuffer)
{
    VCommandBuffer = commandBuffer;
}

CommandBuffer* VulkanCore::IGetCommandBuffer()
{
    return VCommandBuffer;
}

void VulkanCore::ISetPipeline(Pipelines* pipeline)
{
    VPipeline = pipeline;
}

Pipelines* VulkanCore::IGetPipeline()
{
    return VPipeline;
}