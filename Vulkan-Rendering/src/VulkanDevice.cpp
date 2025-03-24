#include "VulkanDevice.h"

#include "DeviceQuery.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Pipelines.h"
#include "CommandBuffer.h"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<VulkanDevice*>(glfwGetWindowUserPointer(window));
    app->GetVulkanCore()->GetCommandBuffer()->SetFrameBufferResized(true);
}

VulkanDevice::VulkanDevice()
{
    VInstance = new VulkanInstance();
    VSurface = new VulkanSurface();
    VDeviceQuery = new DeviceQuery();
    VSwapChain = new SwapChain();
    VRenderPass = new RenderPass();
    VCommandBuffer = new CommandBuffer();
    VPipeline = new Pipelines();
    VCore = new VulkanCore();
}

VulkanDevice::~VulkanDevice()
{
    VInstance = nullptr;
    VSurface = nullptr;
    VDeviceQuery = nullptr;
    VSwapChain = nullptr;
    VRenderPass = nullptr;
    VCommandBuffer = nullptr;
    VPipeline = nullptr;
    VCore = nullptr;
}

void VulkanDevice::RunApplication()
{
    InitWindow();
    InitVulkan();
    MainLoop();
    CleanUp();
}

VulkanCore* VulkanDevice::GetVulkanCore()
{
    return VCore;
}

void VulkanDevice::InitWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "LW - Vulkan Renderer", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void VulkanDevice::InitVulkan()
{
    VCore->SetInstance(VInstance);
    VInstance->CreateVulkanInstance();
    VInstance->SetupDebugMessenger();

    VCore->SetSurface(VSurface);
    VSurface->CreateSurface(VCore, window);

    VCore->SetDevice(VDeviceQuery);
    VDeviceQuery->PickPhysicalDevice(VCore);
    VDeviceQuery->CreateLogicalDevice(VCore);

    VCore->SetSwapChain(VSwapChain);
    VSwapChain->CreateSwapChain(window, VCore);
    VSwapChain->CreateImageViews(VCore);

    VCore->SetRenderPass(VRenderPass);
    VRenderPass->CreateRenderPass(VCore);
    VPipeline->CreateGraphicsPipeline(VCore);
    
    VSwapChain->CreateFrameBuffers(VCore);

    VCore->SetCommandBuffer(VCommandBuffer);
    VCommandBuffer->CreateCommandPool(VCore);

    VCore->SetPipeline(VPipeline);
    VPipeline->CreateVertexBuffer(VCore);
    VPipeline->CreateIndexBuffer(VCore);
    VCommandBuffer->CreateCommandBuffers(VCore);
    VCommandBuffer->CreateSyncObjects(VCore);
}

void VulkanDevice::MainLoop()
{
    while(glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
        VCommandBuffer->DrawFrame(window, VCore);
    }

    vkDeviceWaitIdle(VDeviceQuery->GetChosenGPUDevice());
}

void VulkanDevice::CleanUp() const
{
    VCommandBuffer->CleanUp(*VDeviceQuery);

    VSwapChain->CleanUpSwapChain(VCore);

    VPipeline->CleanUp(*VDeviceQuery);

    vkDestroyRenderPass(VDeviceQuery->GetChosenGPUDevice(), VRenderPass->GetRenderPass(), nullptr);
    
    vkDestroyDevice(VDeviceQuery->GetChosenGPUDevice(), nullptr);
    
    if (VInstance->GetEnableValidation())
    {
        DestroyDebugUtilsMessengerEXT(VInstance->GetVulkanInstance(), VInstance->GetDebugMessenger(), nullptr);
    }

    vkDestroySurfaceKHR(VInstance->GetVulkanInstance(), VSurface->GetVulkanSurface(), nullptr);
    
    vkDestroyInstance(VInstance->GetVulkanInstance(), nullptr);
    
    glfwDestroyWindow(window);

    glfwTerminate();
}