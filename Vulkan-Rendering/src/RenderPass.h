#pragma once

#include "VulkanIncludes.h"

class VulkanCore;

class RenderPass
{
public:

    RenderPass();
    ~RenderPass();
    
    void CreateRenderPass(VulkanCore* vulkanCore);

    VkRenderPass GetRenderPass() const;
private:

    VkRenderPass renderPass;
};
