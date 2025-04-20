#include "VertexBuffer.h"

//Project Includes
#include <stdexcept>

#include "VulkanCore.h"
#include "DeviceQuery.h"
#include "CommandBuffer.h"

VertexBuffer::VertexBuffer()
{
}

void VertexBuffer::CreateVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    
    void* data;
    vkMapMemory(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, m_vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), stagingBufferMemory);
    
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

    CopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

    vkDestroyBuffer(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), stagingBuffer, nullptr);
    vkFreeMemory(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), stagingBufferMemory, nullptr);
}

std::vector<Vertex>& VertexBuffer::GetVertices()
{
    return m_vertices;
}

VkBuffer& VertexBuffer::GetVertexBuffer()
{
    return m_vertexBuffer;
}

void VertexBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Create Buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Allocate Buffer!");
    }

    vkBindBufferMemory(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), buffer, bufferMemory, 0);
}

void VertexBuffer::CopyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = VulkanCore::GetCommandBuffer()->BeginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    VulkanCore::GetCommandBuffer()->EndSingleTimeCommands(commandBuffer);
}

void VertexBuffer::CleanUp()
{
    vkDestroyBuffer(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), m_vertexBuffer, nullptr);
    vkFreeMemory(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), m_vertexBufferMemory, nullptr);
}

uint32_t VertexBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(VulkanCore::GetChosenDevice()->GetPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("Failed to find a suitable Memory Type!");
}
