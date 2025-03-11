#pragma once

#include "VulkanIncludes.h"

class DeviceQuery;
class SwapChain;

class RenderPass
{
public:

    RenderPass();
    ~RenderPass();
    
    void CreateRenderPass(DeviceQuery device, SwapChain swapChain);

    VkRenderPass GetRenderPass() const;
private:

    VkRenderPass renderPass;
};
