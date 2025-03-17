#pragma once

#include "VulkanIncludes.h"

#include <vector>
#include <fstream>

class DeviceQuery;
class RenderPass;

//Replace into class later
#include <glm/glm.hpp>
#include <array>

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        
        return attributeDescriptions;
    }
};

const std::vector<Vertex> vertices =
{
    {{0.0f, -0.5f},{1.0f, 1.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f ,0.5f}, {0.0f, 0.0f, 1.0f}}
};

class Pipelines
{
public:
    Pipelines();
    ~Pipelines();

    void CreateGraphicsPipeline(DeviceQuery& deviceQuery, RenderPass& renderPass);

    void CreateVertexBuffer(DeviceQuery& device);

    VkPipelineLayout& GetPipelineLayout();
    VkPipeline& GetGraphicsPipeline();
    VkBuffer& GetVertexBuffer();

    void CleanUp(DeviceQuery& device);
    
private:

    static std::vector<char> ReadFile(const std::string& fileName);
    VkShaderModule CreateShaderModule(const std::vector<char>& code, DeviceQuery& deviceQury);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, DeviceQuery& device);
    
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
};
