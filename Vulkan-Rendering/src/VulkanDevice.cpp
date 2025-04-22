#include "VulkanDevice.h"

//Project Includes
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
    m_instance = new VulkanInstance();
    m_surface = new VulkanSurface();
    m_deviceQuery = new DeviceQuery();
    m_swapChain = new SwapChain();
    m_renderPass = new RenderPass();
    m_commandBuffer = new CommandBuffer();
    m_pipeline = new Pipelines();
    m_camera = new Camera();
    
    m_castleModel = new ModelLoader();
    m_models = new std::vector<ModelLoader>();
    m_models->push_back(*m_castleModel);
    
}

VulkanDevice::~VulkanDevice()
{
    m_instance = nullptr;
    m_surface = nullptr;
    m_deviceQuery = nullptr;
    m_swapChain = nullptr;
    m_renderPass = nullptr;
    m_commandBuffer = nullptr;
    m_pipeline = nullptr;
    m_camera = nullptr;
    m_castleModel = nullptr;
    m_models = nullptr;
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

void VulkanDevice::InitVulkan() const
{
    VulkanCore::SetInstance(m_instance);
    VulkanCore::SetSurface(m_surface);
    VulkanCore::SetDevice(m_deviceQuery);
    VulkanCore::SetSwapChain(m_swapChain);
    VulkanCore::SetRenderPass(m_renderPass);
    VulkanCore::SetPipeline(m_pipeline);
    VulkanCore::SetCommandBuffer(m_commandBuffer);
    VulkanCore::SetCamera(m_camera);
    VulkanCore::SetModels(m_models);
    
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
    
    m_castleModel->LoadModel("models/Castle/castle.obj");
    m_castleModel->CreateModelBuffers();
    
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