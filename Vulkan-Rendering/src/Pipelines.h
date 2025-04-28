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
    void CreateUniformBuffers();
    void UpdateUniformBuffer(uint32_t currentFrame) const;
    void CreateDescriptorPool();
    void CreateDescriptorSets();

    VkPipelineLayout& GetPipelineLayout();
    VkPipeline& GetGraphicsPipeline();

    //Fix This
    std::vector<VkDescriptorSet>& GetDescriptorSets();

    void CleanUp() const;

private:
    
    static std::vector<char> ReadFile(const std::string& fileName);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
};
