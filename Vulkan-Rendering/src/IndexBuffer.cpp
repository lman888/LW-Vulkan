#include "IndexBuffer.h"

//Project Includes
#include "DeviceQuery.h"
#include "ModelLoader.h"
#include "VertexBuffer.h"
#include "VulkanCore.h"

IndexBuffer::IndexBuffer()
{
}

void IndexBuffer::CreateIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

    for (ModelLoader& model : *VulkanCore::GetModels())
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        model.GetModelVertex().CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        void* data;
        vkMapMemory(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_indices.data(), (size_t)bufferSize);
        vkUnmapMemory(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), stagingBufferMemory);

        model.GetModelVertex().CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);
        
        model.GetModelVertex().CopyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

        vkDestroyBuffer(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), stagingBuffer, nullptr);
        vkFreeMemory(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), stagingBufferMemory, nullptr);
    }
}

void IndexBuffer::CleanUp() const
{
    vkDestroyBuffer(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), m_indexBuffer, nullptr);
    vkFreeMemory(VulkanCore::GetChosenDevice()->GetChosenGPUDevice(), m_indexBufferMemory, nullptr);
}

std::vector<uint32_t>& IndexBuffer::GetIndices()
{
    return m_indices;
}

VkBuffer& IndexBuffer::GetIndexBuffer()
{
    return m_indexBuffer;
}
