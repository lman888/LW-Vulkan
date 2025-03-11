#pragma once

#include "VulkanIncludes.h"

#include <vector>
#include <fstream>

class DeviceQuery;
class RenderPass;

class Pipelines
{
public:
    Pipelines();
    ~Pipelines();

    void CreateGraphicsPipeline(DeviceQuery deviceQuery, RenderPass renderPass);

    VkPipelineLayout& GetPipelineLayout();
    VkPipeline& GetGraphicsPipeline();
    
private:

    static std::vector<char> ReadFile(const std::string& fileName);
    VkShaderModule CreateShaderModule(const std::vector<char>& code, DeviceQuery deviceQury);
    
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
};
