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
    void CreateIndexBuffer();
    void CreateUniformBuffers();
    void UpdateUniformBuffer(uint32_t currentFrame) const;
    void CreateDescriptorPool();
    void CreateDescriptorSets();
    void CreateTextureImage(const std::string texture);
    void CreateTextureImageView();
    void CreateTextureSampler();
    void CreateDepthResource();
    
    VkImageView CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags flags, uint32_t mipLevels); 

    std::vector<uint32_t>& GetIndices();
    VkPipelineLayout& GetPipelineLayout();
    VkPipeline& GetGraphicsPipeline();
    VkBuffer& GetIndexBuffer();
    VkImageView& GetImageView();
    VkImage& GetImage();
    VkDeviceMemory& GetImageMemory();

    //Fix This
    uint32_t GetMipLevels();
    std::vector<VkDescriptorSet>& GetDescriptorSets();

    void CleanUp() const;

private:

    void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    void TransitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    void CopyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);

    void GenerateMipmaps(VkImage& image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    
    static std::vector<char> ReadFile(const std::string& fileName);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    
    std::vector<uint32_t> indices;
    
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
