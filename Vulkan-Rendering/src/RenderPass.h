#pragma once

#include "VulkanIncludes.h"

class RenderPass
{
public:

    RenderPass();
    ~RenderPass();
    
    void CreateRenderPass();

    VkRenderPass GetVulkanRenderPass() const;
private:

    VkRenderPass renderPass;
};
