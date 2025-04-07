#pragma once

#include "VulkanIncludes.h"

#include <vector>
#include <fstream>

class DeviceQuery;
class RenderPass;
class CommandBuffer;

//Replace into class later
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <array>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // Changed to support Vec 3 position input
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
        
        return attributeDescriptions;
    }
};

const std::vector<Vertex> vertices =
{
    //Front Face
    {{-0.5f, -0.5f, 0.5f},{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f ,0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f ,0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f},{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f},{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    {{0.5f ,0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
    { {-0.5f ,0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}
};

const std::vector<uint16_t> indices =
{
    0, 1, 2, 2, 3, 0, //Front
    4, 5, 6, 6, 7, 4, //Back
    0, 4, 7, 7, 3, 0, //Left
    1, 5, 6, 6, 2, 1,  // Right
    3, 2, 6, 6, 7, 3,  // Top
    0, 1, 5, 5, 4, 0   // Bottom
};

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class Pipelines
{
public:
    Pipelines();
    ~Pipelines();

    //Need to move into a Shader Class
    void CreateDescriptorSetLayout();

    void CreateGraphicsPipeline();

    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateUniformBuffers();
    void UpdateUniformBuffer(uint32_t currentFrame) const;
    void CreateDescriptorPool();
    void CreateDescriptorSets();
    void CreateTextureImage();
    void CreateTextureImageView();
    void CreateTextureSampler();
    
    VkImageView CreateImageView(VkImage image, VkFormat format); 

    VkPipelineLayout& GetPipelineLayout();
    VkPipeline& GetGraphicsPipeline();
    VkBuffer& GetVertexBuffer();
    VkBuffer& GetIndexBuffer();
    std::vector<VkDescriptorSet>& GetDescriptorSets();

    void CleanUp() const;

private:

    static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    
    static std::vector<char> ReadFile(const std::string& fileName);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

};
