#include "CommandBuffer.h"

#include <filesystem>
#include <array>

//Project Includes
#include "Camera.h"
#include "DeviceQuery.h"
#include "Pipelines.h"
#include "RenderPass.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
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
void CommandBuffer::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = VulkanCore::GetChosenDevice()->FindQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Command Pool!");
    }
}

/*
 * Commands in Vulkan, like drawing operations and memory transfers, are not executed directly using function calls. We have to record all the operations we want to perform in command buffer objects.
 * The advantage to this is when we are ready to tell Vulkan what we want to do, all the commands are submitted together and Vulkan can more efficiently process the commands since all of them are available together.
 * This also allows commands to be recorded to happen in multiple threads if we desire.
 */
void CommandBuffer::CreateCommandBuffers()
{
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();
    
    if (vkAllocateCommandBuffers(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate Command Buffers!");
    }
}

void CommandBuffer::RecordCommandBuffer(uint32_t imageIndex)
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
    renderPassInfo.renderPass = VulkanCore::GetRenderPass()->GetVulkanRenderPass();
    renderPassInfo.framebuffer = VulkanCore::GetSwapChain()->GetSwapChainFrameBuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = VulkanCore::GetSwapChain()->GetSwapChainImageExtent();
    
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanCore::GetPipeline()->GetGraphicsPipeline());

    VkBuffer vertexBuffers[] = { VulkanCore::GetVertexBuffer()->GetVertexBuffer() };
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffers[currentFrame], VulkanCore::GetPipeline()->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
    
    VkViewport viewPort{};
    viewPort.x = 0.0f;
    viewPort.y = 0.0f;
    viewPort.width = (float)VulkanCore::GetSwapChain()->GetSwapChainImageExtent().width;
    viewPort.height = (float)VulkanCore::GetSwapChain()->GetSwapChainImageExtent().height;
    viewPort.minDepth = 0.0f;
    viewPort.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewPort);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = VulkanCore::GetSwapChain()->GetSwapChainImageExtent();
    vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);

    vkCmdBindDescriptorSets(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanCore::GetPipeline()->GetPipelineLayout(), 0, 1, &VulkanCore::GetPipeline()->GetDescriptorSets()[currentFrame], 0, nullptr);
    
    vkCmdDrawIndexed(commandBuffers[currentFrame], static_cast<uint32_t>(VulkanCore::GetPipeline()->GetIndices().size()), 1, 0, 0, 0);
    
    vkCmdEndRenderPass(commandBuffers[currentFrame]);

    if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Record Command Buffer!");
    }
}

void CommandBuffer::CreateSyncObjects()
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
        if (vkCreateSemaphore(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Semaphores!");
        }
    }
}

void CommandBuffer::CleanUp() const
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), commandPool, nullptr);
}

/*
 * Rendering a frame in Vulkan consists of a common set of steps:
 * - Wait for the previous frame to finish
 * - Acquire an image from the swap chain
 * - Record a command buffer which draws the scene onto that image
 * - Submit the recorded command buffer
 * - Present the swap chain image
 */
void CommandBuffer::DrawFrame(GLFWwindow* window)
{
    vkWaitForFences(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), VulkanCore::GetSwapChain()->GetVulkanSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        VulkanCore::GetSwapChain()->ReCreateSwapChain(window);
        return;
    }
    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire Swap Chain Image!");
    }
    
    VulkanCore::GetPipeline()->UpdateUniformBuffer(currentFrame);
    
    vkResetFences(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), 1, &inFlightFences[currentFrame]);
    vkResetCommandBuffer(commandBuffers[currentFrame], 0);

    RecordCommandBuffer(imageIndex);

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

    if (vkQueueSubmit(VulkanCore::GetChosenDevice()->GetGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit Draw Command Buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { VulkanCore::GetSwapChain()->GetVulkanSwapChain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; //Optional
    
    result = vkQueuePresentKHR(VulkanCore::GetChosenDevice()->GetPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResized)
    {
        frameBufferResized = false;
        VulkanCore::GetSwapChain()->ReCreateSwapChain(window);
    }
    else if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to acquire Swap Chain Image!");
    }
    
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VkCommandBuffer CommandBuffer::BeginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &allocInfo, &commandBuffer);
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void CommandBuffer::EndSingleTimeCommands(VkCommandBuffer& commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(VulkanCore::GetChosenDevice()->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(VulkanCore::GetChosenDevice()->GetGraphicsQueue());

    vkFreeCommandBuffers(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), commandPool, 1, &commandBuffer);
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