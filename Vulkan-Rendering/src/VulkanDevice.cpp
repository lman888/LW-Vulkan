#include "VulkanDevice.h"

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<VulkanDevice*>(glfwGetWindowUserPointer(window));
    app->VCommandBuffer.SetFrameBufferResized(true);
}

VulkanDevice::VulkanDevice()
{
}

VulkanDevice::~VulkanDevice()
{
}

void VulkanDevice::RunApplication()
{
    InitWindow();
    InitVulkan();
    MainLoop();
    CleanUp();
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
    VInstance.CreateVulkanInstance();
    VInstance.SetupDebugMessenger();
    VSurface.CreateSurface(VInstance.GetVulkanInstance(), window);
    VDeviceQuery.PickPhysicalDevice(VInstance.GetVulkanInstance(), VSurface.GetVulkanSurface(), VSwapChain);
    VDeviceQuery.CreateLogicalDevice(VSurface.GetVulkanSurface(), VInstance.GetEnableValidation(), VInstance.GetValidationLayers());
    VSwapChain.CreateSwapChain(VDeviceQuery, VSurface, window);
    VSwapChain.CreateImageViews(VDeviceQuery);
    VRenderPass.CreateRenderPass(VDeviceQuery, VSwapChain);
    VPipeline.CreateGraphicsPipeline(VDeviceQuery, VRenderPass);
    VSwapChain.CreateFrameBuffers(VDeviceQuery, VRenderPass);
    VCommandBuffer.CreateCommandPool(VDeviceQuery, VSurface);
    VPipeline.CreateVertexBuffer(VDeviceQuery, VCommandBuffer);
    VPipeline.CreateIndexBuffer(VDeviceQuery, VCommandBuffer);
    VCommandBuffer.CreateCommandBuffers(VDeviceQuery);
    VCommandBuffer.CreateSyncObjects(VDeviceQuery);
}

void VulkanDevice::MainLoop()
{
    while(glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
        VCommandBuffer.DrawFrame(VDeviceQuery, VSwapChain, VPipeline, VRenderPass, VSurface, window);
    }

    vkDeviceWaitIdle(VDeviceQuery.GetChosenDevice());
}

void VulkanDevice::CleanUp()
{
    VCommandBuffer.CleanUp(VDeviceQuery);

    VSwapChain.CleanUpSwapChain(VDeviceQuery);

    VPipeline.CleanUp(VDeviceQuery);

    vkDestroyRenderPass(VDeviceQuery.GetChosenDevice(), VRenderPass.GetRenderPass(), nullptr);
    
    vkDestroyDevice(VDeviceQuery.GetChosenDevice(), nullptr);
    
    if (VInstance.GetEnableValidation())
    {
        DestroyDebugUtilsMessengerEXT(VInstance.GetVulkanInstance(), VInstance.GetDebugMessenger(), nullptr);
    }

    vkDestroySurfaceKHR(VInstance.GetVulkanInstance(), VSurface.GetVulkanSurface(), nullptr);
    
    vkDestroyInstance(VInstance.GetVulkanInstance(), nullptr);
    
    glfwDestroyWindow(window);

    glfwTerminate();
}