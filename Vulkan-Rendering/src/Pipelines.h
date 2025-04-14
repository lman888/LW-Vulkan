#pragma once

#include "VulkanIncludes.h"

#include <vector>
#include <fstream>

class DeviceQuery;
class RenderPass;
class CommandBuffer;

//Replace into class later
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
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
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
        
        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

namespace std 
{
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

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
    void CreateTextureImage(const std::string texture);
    void CreateTextureImageView();
    void CreateTextureSampler();
    void CreateDepthResource();

    void LoadModel(std::string modelPath);
    
    VkImageView CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags flags, uint32_t mipLevels); 

    const std::vector<uint32_t>& GetIndices() const;
    VkPipelineLayout& GetPipelineLayout();
    VkPipeline& GetGraphicsPipeline();
    VkBuffer& GetVertexBuffer();
    VkBuffer& GetIndexBuffer();
    VkImageView& GetImageView();
    VkImage& GetImage();
    VkDeviceMemory& GetImageMemory();

    //Fix This
    uint32_t GetMipLevels();
    std::vector<VkDescriptorSet>& GetDescriptorSets();

    void CleanUp() const;

private:

    static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void CopyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);

    void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    void TransitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    void CopyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);

    void GenerateMipmaps(VkImage& image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    
    static std::vector<char> ReadFile(const std::string& fileName);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t mipLevels;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
};
