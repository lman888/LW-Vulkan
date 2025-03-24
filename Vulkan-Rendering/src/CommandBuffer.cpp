#include "CommandBuffer.h"

#include <filesystem>

#include "DeviceQuery.h"
#include "Pipelines.h"
#include "RenderPass.h"
#include "VulkanSurface.h"
#include "SwapChain.h"
#include "VulkanCore.h"

CommandBuffer::CommandBuffer()
{
}

CommandBuffer::~CommandBuffer()
{
}

/*
 * The Command Pool manages the memory that is used to store the buffers and command buffers are allocated from there.
 */
void CommandBuffer::CreateCommandPool(VulkanCore* vulkanCore)
{
    QueueFamilyIndices queueFamilyIndices = vulkanCore->GetChosenDevice()->FindQueueFamilies(vulkanCore);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(vulkanCore->GetChosenDevice()->GetChosenGPUDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Command Pool!");
    }
}

/*
 * Commands in Vulkan, like drawing operations and memory transfers, are not executed directly using function calls. We have to record all the operations we want to perform in command buffer objects.
 * The advantage to this is when we are ready to tell Vulkan what we want to do, all the commands are submitted together and Vulkan can more efficiently process the commands since all of them are available together.
 * This also allows commands to be recorded to happen in multiple threads if we desire.
 */
void CommandBuffer::CreateCommandBuffers(VulkanCore* vulkanCore)
{
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();
    
    if (vkAllocateCommandBuffers(vulkanCore->GetChosenDevice()->GetChosenGPUDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate Command Buffers!");
    }
}

void CommandBuffer::RecordCommandBuffer(uint32_t imageIndex, VulkanCore* vulkanCore)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; //Optional
    beginInfo.pInheritanceInfo = nullptr; //Optional

    if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin Recording Command Buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanCore->GetRenderPass()->GetRenderPass();
    renderPassInfo.framebuffer = vulkanCore->GetSwapChain()->GetSwapChainFrameBuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vulkanCore->GetSwapChain()->GetSwapChainImageExtent();

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanCore->GetPipeline()->GetGraphicsPipeline());

    VkBuffer vertexBuffers[] = { vulkanCore->GetPipeline()->GetVertexBuffer() };
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffers[currentFrame], vulkanCore->GetPipeline()->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
    
    VkViewport viewPort{};
    viewPort.x = 0.0f;
    viewPort.y = 0.0f;
    viewPort.width = (float)vulkanCore->GetSwapChain()->GetSwapChainImageExtent().width;
    viewPort.height = (float)vulkanCore->GetSwapChain()->GetSwapChainImageExtent().height;
    viewPort.minDepth = 0.0f;
    viewPort.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewPort);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = vulkanCore->GetSwapChain()->GetSwapChainImageExtent();
    vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);
    
    vkCmdDrawIndexed(commandBuffers[currentFrame], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
    
    vkCmdEndRenderPass(commandBuffers[currentFrame]);

    if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Record Command Buffer!");
    }
}

void CommandBuffer::CreateSyncObjects(VulkanCore* vulkanCore)
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(vulkanCore->GetChosenDevice()->GetChosenGPUDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(vulkanCore->GetChosenDevice()->GetChosenGPUDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(vulkanCore->GetChosenDevice()->GetChosenGPUDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Semaphores!");
        }
    }
}

void CommandBuffer::CleanUp(DeviceQuery& device) const
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device.GetChosenGPUDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device.GetChosenGPUDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(device.GetChosenGPUDevice(), inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device.GetChosenGPUDevice(), commandPool, nullptr);
}

/*
 * Rendering a frame in Vulkan consists of a common set of steps:
 * - Wait for the previous frame to finish
 * - Acquire an image from the swap chain
 * - Record a command buffer which draws the scene onto that image
 * - Submit the recorded command buffer
 * - Present the swap chain image
 */
void CommandBuffer::DrawFrame(GLFWwindow* window, VulkanCore* vulkanCore)
{
    vkWaitForFences(vulkanCore->GetChosenDevice()->GetChosenGPUDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(vulkanCore->GetChosenDevice()->GetChosenGPUDevice(), vulkanCore->GetSwapChain()->GetSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        vulkanCore->GetSwapChain()->ReCreateSwapChain(window, vulkanCore);
        return;
    }
    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire Swap Chain Image!");
    }
    
    vkResetFences(vulkanCore->GetChosenDevice()->GetChosenGPUDevice(), 1, &inFlightFences[currentFrame]);
    vkResetCommandBuffer(commandBuffers[currentFrame], 0);

    RecordCommandBuffer(imageIndex, vulkanCore);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkanCore->GetChosenDevice()->GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit Draw Command Buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { vulkanCore->GetSwapChain()->GetSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; //Optional
    
    result = vkQueuePresentKHR(vulkanCore->GetChosenDevice()->GetPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResized)
    {
        frameBufferResized = false;
        vulkanCore->GetSwapChain()->ReCreateSwapChain(window, vulkanCore);
    }
    else if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to acquire Swap Chain Image!");
    }
    
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VkCommandBuffer& CommandBuffer::GetCommandBuffer()
{
    return commandBuffers[currentFrame];
}

VkCommandPool& CommandBuffer::GetCommandPool()
{
    return commandPool;
}

void CommandBuffer::SetFrameBufferResized(bool resized)
{
    frameBufferResized = resized;
}