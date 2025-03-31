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
    app->GetVulkanCommandBuffer()->SetFrameBufferResized(true);
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
}

void VulkanDevice::RunApplication()
{
    InitWindow();
    InitVulkan();
    MainLoop();
    CleanUp();
}

CommandBuffer* VulkanDevice::GetVulkanCommandBuffer()
{
    return VulkanCore::Get().GetCommandBuffer();
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
    VulkanCore::SetInstance(VInstance);
    VulkanCore::SetSurface(VSurface);
    VulkanCore::SetDevice(VDeviceQuery);
    VulkanCore::SetSwapChain(VSwapChain);
    VulkanCore::SetRenderPass(VRenderPass);
    VulkanCore::SetPipeline(VPipeline);
    VulkanCore::SetCommandBuffer(VCommandBuffer);
    
    VulkanCore::GetInstance()->CreateVulkanInstance();
    VulkanCore::GetInstance()->SetupDebugMessenger();

    VulkanCore::GetSurface()->CreateSurface(window);

    VulkanCore::GetChosenDevice()->PickPhysicalDevice();
    VulkanCore::GetChosenDevice()->CreateLogicalDevice();

    VulkanCore::GetSwapChain()->CreateSwapChain(window);
    VulkanCore::GetSwapChain()->CreateImageViews();

    VulkanCore::GetRenderPass()->CreateRenderPass();
    VulkanCore::GetPipeline()->CreateDescriptorSetLayout();
    VulkanCore::GetPipeline()->CreateGraphicsPipeline();
    
    VulkanCore::GetSwapChain()->CreateFrameBuffers();

    VulkanCore::GetCommandBuffer()->CreateCommandPool();

    VulkanCore::GetPipeline()->CreateVertexBuffer();
    VulkanCore::GetPipeline()->CreateIndexBuffer();
    VulkanCore::GetPipeline()->CreateUniformBuffers();
    VulkanCore::GetPipeline()->CreateDescriptorPool();
    VulkanCore::GetPipeline()->CreateDescriptorSets();
    VulkanCore::GetCommandBuffer()->CreateCommandBuffers();
    VulkanCore::GetCommandBuffer()->CreateSyncObjects();
}

void VulkanDevice::MainLoop()
{
    while(glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
        VulkanCore::GetCommandBuffer()->DrawFrame(window);
    }

    vkDeviceWaitIdle(VulkanCore::GetChosenDevice()->GetChosenGPUDevice());
}

void VulkanDevice::CleanUp() const
{
    VulkanCore::GetCommandBuffer()->CleanUp();

    VulkanCore::GetSwapChain()->CleanUpSwapChain();

    VulkanCore::GetPipeline()->CleanUp();

    vkDestroyRenderPass(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), VulkanCore::GetRenderPass()->GetVulkanRenderPass(), nullptr);
    
    vkDestroyDevice(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), nullptr);
    
    if (VulkanCore::GetInstance()->GetEnableValidation())
    {
        DestroyDebugUtilsMessengerEXT(VulkanCore::GetInstance()->GetVulkanInstance(), VulkanCore::GetInstance()->GetDebugMessenger(), nullptr);
    }

    vkDestroySurfaceKHR(VulkanCore::GetInstance()->GetVulkanInstance(), VulkanCore::GetSurface()->GetVulkanSurface(), nullptr);
    
    vkDestroyInstance(VulkanCore::GetInstance()->GetVulkanInstance(), nullptr);
    
    glfwDestroyWindow(window);

    glfwTerminate();
}