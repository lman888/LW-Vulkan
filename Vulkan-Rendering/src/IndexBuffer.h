#pragma once

#include <vector>

//Project Includes
#include "VulkanIncludes.h"

class IndexBuffer
{
public:
    IndexBuffer();
    ~IndexBuffer() = default;

    void CreateIndexBuffer();

    void CleanUp() const;
    
    std::vector<uint32_t>& GetIndices();
    VkBuffer& GetIndexBuffer();
    
private:

    std::vector<uint32_t> m_indices;
    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferMemory;
};
