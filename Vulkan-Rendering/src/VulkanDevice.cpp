#include "VulkanDevice.h"

#include <iostream>
#include <queue>
#include <cstdint>
#include <complex>

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

    CreateCommandPool();
    CreateCommandBuffer();
    CreateSyncObjects();
}

void VulkanDevice::MainLoop()
{
    while(glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
        DrawFrame();
    }

    vkDeviceWaitIdle(VDeviceQuery.GetChosenDevice());
}

void VulkanDevice::CleanUp()
{
    vkDestroySemaphore(VDeviceQuery.GetChosenDevice(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(VDeviceQuery.GetChosenDevice(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(VDeviceQuery.GetChosenDevice(), inFlightFence, nullptr);
    
    vkDestroyCommandPool(VDeviceQuery.GetChosenDevice(), commandPool, nullptr);
    
    for (auto frameBuffer : VSwapChain.GetSwapChainFrameBuffers())
    {
        vkDestroyFramebuffer(VDeviceQuery.GetChosenDevice(), frameBuffer, nullptr);
    }
    
    vkDestroyPipeline(VDeviceQuery.GetChosenDevice(), VPipeline.GetGraphicsPipeline(), nullptr);
    vkDestroyPipelineLayout(VDeviceQuery.GetChosenDevice(), VPipeline.GetPipelineLayout(), nullptr);

    vkDestroyRenderPass(VDeviceQuery.GetChosenDevice(), VRenderPass.GetRenderPass(), nullptr);

    for (auto imageView : VSwapChain.GetSwapChainImageViews())
    {
        vkDestroyImageView(VDeviceQuery.GetChosenDevice(), imageView, nullptr);
    }
    
    vkDestroySwapchainKHR(VDeviceQuery.GetChosenDevice(), VSwapChain.GetSwapChain(), nullptr);
    
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

/*
 * The Command Pool manages the memory that is used to store the buffers and command buffers are allocated from there.
 */
void VulkanDevice::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = VDeviceQuery.FindQueueFamilies(VDeviceQuery.GetPhysicalDevice(), VSurface.GetVulkanSurface());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(VDeviceQuery.GetChosenDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Command Pool!");
    }
}

/*
 * Commands in Vulkan, like drawing operations and memory transfers, are not executed directly using function calls. We have to record all the operations we want to perform in command buffer objects.
 * The advantage to this is when we are ready to tell Vulkan what we want to do, all the commands are submitted together and Vulkan can more efficiently process the commands since all of them are available together.
 * This also allows commands to be recorded to happen in multiple threads if we desire.
 */
void VulkanDevice::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(VDeviceQuery.GetChosenDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate Command Buffers!");
    }
}

void VulkanDevice::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; //Optional
    beginInfo.pInheritanceInfo = nullptr; //Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin Recording Command Buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = VRenderPass.GetRenderPass();
    renderPassInfo.framebuffer = VSwapChain.GetSwapChainFrameBuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VSwapChain.GetSwapChainImageExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VPipeline.GetGraphicsPipeline());

    VkViewport viewPort{};
    viewPort.x = 0.0f;
    viewPort.y = 0.0f;
    viewPort.width = (float)VSwapChain.GetSwapChainImageExtent().width;
    viewPort.height = (float)VSwapChain.GetSwapChainImageExtent().height;
    viewPort.minDepth = 0.0f;
    viewPort.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewPort);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VSwapChain.GetSwapChainImageExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Record Command Buffer!");
    }
}

/*
 * Rendering a frame in Vulkan consists of a common set of steps:
 * - Wait for the previous frame to finish
 * - Acquire an image from the swap chain
 * - Record a command buffer which draws the scene onto that image
 * - Submit the recorded command buffer
 * - Present the swap chain image
 */
void VulkanDevice::DrawFrame()
{
    vkWaitForFences(VDeviceQuery.GetChosenDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    vkResetFences(VDeviceQuery.GetChosenDevice(), 1, &inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(VDeviceQuery.GetChosenDevice(), VSwapChain.GetSwapChain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffer, 0);

    RecordCommandBuffer(commandBuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(VDeviceQuery.GetGraphicsQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit Draw Command Buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { VSwapChain.GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; //Optional

    vkQueuePresentKHR(VDeviceQuery.GetPresentQueue(), &presentInfo);
}

void VulkanDevice::CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    if (vkCreateSemaphore(VDeviceQuery.GetChosenDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(VDeviceQuery.GetChosenDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(VDeviceQuery.GetChosenDevice(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Semaphores!");
    }
}