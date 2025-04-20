#include "VulkanDevice.h"

#include "DeviceQuery.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Pipelines.h"
#include "CommandBuffer.h"
#include "Camera.h"
#include "ModelLoader.h"

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
    VCamera = new Camera();
    VVertexBuffer = new VertexBuffer();
    CastleModel = new ModelLoader();
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
    VVertexBuffer = nullptr;
    CastleModel = nullptr;
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
    
    window = glfwCreateWindow(WIDTH, HEIGHT, "LW - Vulkan Renderer", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    glfwSetKeyCallback(window, KeyInputCallback);
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
    VulkanCore::SetCamera(VCamera);
    VulkanCore::SetVertexBuffer(VVertexBuffer);
    
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

    VulkanCore::GetCommandBuffer()->CreateCommandPool();
    VulkanCore::GetPipeline()->CreateDepthResource();
    VulkanCore::GetSwapChain()->CreateFrameBuffers();
    VulkanCore::GetPipeline()->CreateTextureImage("textures/viking_room.png");
    VulkanCore::GetPipeline()->CreateTextureImageView();
    VulkanCore::GetPipeline()->CreateTextureSampler();
    
    CastleModel->LoadModel("models/Castle/castle.obj");
    VulkanCore::GetVertexBuffer()->CreateVertexBuffer();
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