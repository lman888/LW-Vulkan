#include "VulkanDevice.h"

#include <iostream>
#include <queue>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <complex>

VulkanDevice::VulkanDevice()
{
}

VulkanDevice::~VulkanDevice()
{
}

void VulkanDevice::RunApplication()
{
    InitWindow();
    InitVulkan();
    MainLoop();
    CleanUp();
}

void VulkanDevice::InitWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "LW - Vulkan Renderer", nullptr, nullptr);
}

void VulkanDevice::InitVulkan()
{
    VInstance.CreateVulkanInstance();
    VInstance.SetupDebugMessenger();
    VSurface.CreateSurface(VInstance.GetVulkanInstance(), window);
    VdeviceQuery.PickPhysicalDevice(VInstance.GetVulkanInstance(), VSurface.GetVulkanSurface());
    VdeviceQuery.CreateLogicalDevice(VSurface.GetVulkanSurface(), VInstance.GetEnableValidation(), VInstance.GetValidationLayers());
    
    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateFrameBuffers();
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSyncObjects();
}

void VulkanDevice::MainLoop()
{
    while(glfwWindowShouldClose(window) == false)
    {
        glfwPollEvents();
        DrawFrame();
    }

    vkDeviceWaitIdle(VdeviceQuery.GetChosenDevice());
}

void VulkanDevice::CleanUp()
{
    vkDestroySemaphore(VdeviceQuery.GetChosenDevice(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(VdeviceQuery.GetChosenDevice(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(VdeviceQuery.GetChosenDevice(), inFlightFence, nullptr);
    
    vkDestroyCommandPool(VdeviceQuery.GetChosenDevice(), commandPool, nullptr);
    
    for (auto frameBuffer : swapChainFrameBuffers)
    {
        vkDestroyFramebuffer(VdeviceQuery.GetChosenDevice(), frameBuffer, nullptr);
    }
    
    vkDestroyPipeline(VdeviceQuery.GetChosenDevice(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(VdeviceQuery.GetChosenDevice(), pipelineLayout, nullptr);

    vkDestroyRenderPass(VdeviceQuery.GetChosenDevice(), renderPass, nullptr);

    for (auto imageView : swapChainImageViews)
    {
        vkDestroyImageView(VdeviceQuery.GetChosenDevice(), imageView, nullptr);
    }
    
    vkDestroySwapchainKHR(VdeviceQuery.GetChosenDevice(), swapChain, nullptr);
    
    vkDestroyDevice(VdeviceQuery.GetChosenDevice(), nullptr);
    
    if (VInstance.GetEnableValidation())
    {
        DestroyDebugUtilsMessengerEXT(VInstance.GetVulkanInstance(), VInstance.GetDebugMessenger(), nullptr);
    }

    vkDestroySurfaceKHR(VInstance.GetVulkanInstance(), VSurface.GetVulkanSurface(), nullptr);
    
    vkDestroyInstance(VInstance.GetVulkanInstance(), nullptr);
    
    glfwDestroyWindow(window);

    glfwTerminate();
}

VkSurfaceFormatKHR VulkanDevice::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    
    return availableFormats[0];
}

VkPresentModeKHR VulkanDevice::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanDevice::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

        actualExtent.width = std::clamp(actualExtent.width,capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        
        return actualExtent;
    }
}

/*
 * Vulkan does not have the concept of a "default framebuffer", hence it requires an infrastructure that will own the buffers we will render to before we visualize them on the screen. This infrastructure is known as Swap Chain.
 * and must be created explicitly by Vulkan.
 *
 * The Swap Chain is essentially a queue of images that are waiting to be presented to the screen. Our application will acquire such an image to draw to it, and then return it to the queue.
 *
 * How exactly the queue works and the conditions for presenting an image from the queue depend on how the swap chain is set up, but the general purpose of the swap chain is to synchronize the presentation of images with the refresh
 * rate of the screen.
 */
void VulkanDevice::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport = VdeviceQuery.QuerySwapChainSupport(VdeviceQuery.GetPhysicalDevice(), VSurface.GetVulkanSurface());

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    //We include the +1 because it may take drivers time to complete internal operations
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = VSurface.GetVulkanSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = VdeviceQuery.FindQueueFamilies(VdeviceQuery.GetPhysicalDevice(), VSurface.GetVulkanSurface());
    uint32_t queueFamilyIndices[] { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; //optional
        createInfo.pQueueFamilyIndices = nullptr; //optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(VdeviceQuery.GetChosenDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Create a Swap Chain!");
    }

    vkGetSwapchainImagesKHR(VdeviceQuery.GetChosenDevice(), swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(VdeviceQuery.GetChosenDevice(), swapChain, &imageCount, swapChainImages.data());
    
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

/*
 * An Image View is quite literally a view into an image. It describes how to access the image and which part of the image to access, for example if it should be treated as a 2D texture depth texture without any mipmapping levels.
 */
void VulkanDevice::CreateImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(VdeviceQuery.GetChosenDevice(), &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Image Views!");
        }
    }
}

/*
 * The Graphics Pipeline is the sequence of operations that take the vertices and textures of our meshes all the way to the pixels in the render targets.
 *
 * A simplified overview of this process is as follows:
 * 1. The Input Assembler collects the raw vertex data from the buffers we specify and may also use an index buffer to repeat certain elements without having to duplicate the vertex data itself.
 *
 * 2. The vertex shader is run for every vertex and generally applies transformations to turn vertex positions from model space to screen space. It also passes per-vertex data down the pipeline.
 *
 * 3. The tesselation shaders allow us to subdivide geometry based on certain rules to increase the mesh quality. This is often used to make surfaces like brick walls and such.
 *
 * 4. The geometry shader is run on every primitive (triangle, line, point) and can discard it or output more primitives than came in. This is similar to the tessellation shader, but much more flexible. However, it is not used much in today's
 * applications because the performance is not that good on most graphics cards expect for Intels Integrated GPUs.
 *
 * 5. The rasterization stage discretizes the primitives into fragments. These are the pixel elements that they fill on the framebuffer. Any fragments that fall outside the screen are discarded and the attributes outputted by the vertex
 * shader are interpolated across the fragments. Usually the fragments that are behind other primitive fragments are also discarded here because of depth testing.
 *
 * 6. The fragment shader is invoked for every fragment that survives and determines which framebuffer(s) that fragments are written to and with which color and depth values. It can do this using the interpolated data from the
 * vertex shader, which can include things like texture coords and normals for lighting.
 *
 * Some stages are programmable, this means we can upload our own code to the graphics card to apply exactly the operations we want. This allows us to use the fragment shaders, as an example to implement anything from texturing and lighting
 * to ray tracers!. These programs run on many GPU cores simultaneously to prcoess many objects, like vertices in parallel.
 * Programmable stages are:
 * - Vertex Shader
 * - Tessellation
 * - Geometry
 * - Fragment Shader
 *
 * Some stages are known as Fixed-Function stages. These stages allow us to tweak their operations using parameters, but the way they work is predefined.
 * Fixed-Function stages are:
 * - Input Assembler
 * - Rasterization
 * - Color Blending
 *
 * In Vulkan we must create the entire graphics pipeline from scratch. The disadvantage of this is how many pipelines we may have to make (A lot of work), the benefits is we have essentially complete control.
 */
void VulkanDevice::CreateGraphicsPipeline()
{
        auto vertShaderCode = ReadFile("shaders/vert.spv");
    auto fragShaderCode = ReadFile("shaders/frag.spv");

    VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    std::vector<VkDynamicState> dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; //Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; //Optional

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    VkPipelineViewportStateCreateInfo viewPortState{};
    viewPortState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewPortState.viewportCount = 1;
    viewPortState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasClamp = 0.0f; //Optional
    rasterizer.depthBiasConstantFactor = 0.0f; //Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; //Optional
    
    VkPipelineMultisampleStateCreateInfo multiSampling{};
    multiSampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSampling.sampleShadingEnable = VK_FALSE;
    multiSampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multiSampling.minSampleShading = 1.0f; //Optional
    multiSampling.pSampleMask = nullptr; //Optional
    multiSampling.alphaToCoverageEnable = VK_FALSE; //Optional
    multiSampling.alphaToOneEnable = VK_FALSE; //Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(VdeviceQuery.GetChosenDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Pipeline Layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewPortState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multiSampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; //Optional
    pipelineInfo.basePipelineIndex = -1; //Optional

    if (vkCreateGraphicsPipelines(VdeviceQuery.GetChosenDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Graphics Pipeline!");
    }
    
    vkDestroyShaderModule(VdeviceQuery.GetChosenDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(VdeviceQuery.GetChosenDevice(), vertShaderModule, nullptr);
}

std::vector<char> VulkanDevice::ReadFile(const std::string& fileName)
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);
    
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open File");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule VulkanDevice::CreateShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(VdeviceQuery.GetChosenDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Shader Module!");
    }

    return shaderModule;
}

/*
 * We need to tell Vulkan about the framebuffer attachments that will be used while rendering. We need to specify how many color and depth buffers there will be, how many samples to use for each of them and how their contents should be handled throughout the rendering operations.
 * All of that information is wrapped into a Render Pass object.
 */
void VulkanDevice::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subPass{};
    subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subPass.colorAttachmentCount = 1;
    subPass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subPass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    
    if (vkCreateRenderPass(VdeviceQuery.GetChosenDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Render Pass!");
    }
}

/*
 * The Frame buffers represent a collection of specific memory attachments that a Render Pass instance uses.
 *
 * A Framebuffer object references all the VkImageView objects that represent the attachments. In this case that will be our color attachment. However, the image that we have to use for the attachment depends on which image
 * the swap chain returns when we retrieve one for presentation. That means that we have to create a framebuffer for all the images in the swap chain and use the one that corresponds to the retrieved image at drawing time.
 */
void VulkanDevice::CreateFrameBuffers()
{
    swapChainFrameBuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainFrameBuffers.size(); i++)
    {
        VkImageView attachments[] = { swapChainImageViews[i] };
        
        VkFramebufferCreateInfo frameBufferInfo{};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = renderPass;
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = swapChainExtent.width;
        frameBufferInfo.height = swapChainExtent.height;
        frameBufferInfo.layers = 1;

        if (vkCreateFramebuffer(VdeviceQuery.GetChosenDevice(), &frameBufferInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create a Framebuffer!");
        }
    }
}

/*
 * The Command Pool manages the memory that is used to store the buffers and command buffers are allocated from there.
 */
void VulkanDevice::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = VdeviceQuery.FindQueueFamilies(VdeviceQuery.GetPhysicalDevice(), VSurface.GetVulkanSurface());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(VdeviceQuery.GetChosenDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Command Pool!");
    }
}

/*
 * Commands in Vulkan, like drawing operations and memory transfers, are not executed directly using function calls. We have to record all the operations we want to perform in command buffer objects.
 * The advantage to this is when we are ready to tell Vulkan what we want to do, all the commands are submitted together and Vulkan can more efficiently process the commands since all of them are available together.
 * This also allows commands to be recorded to happen in multiple threads if we desire.
 */
void VulkanDevice::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(VdeviceQuery.GetChosenDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate Command Buffers!");
    }
}

void VulkanDevice::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; //Optional
    beginInfo.pInheritanceInfo = nullptr; //Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin Recording Command Buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewPort{};
    viewPort.x = 0.0f;
    viewPort.y = 0.0f;
    viewPort.width = (float) swapChainExtent.width;
    viewPort.height = (float) swapChainExtent.height;
    viewPort.minDepth = 0.0f;
    viewPort.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewPort);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Record Command Buffer!");
    }
}

/*
 * Rendering a frame in Vulkan consists of a common set of steps:
 * - Wait for the previous frame to finish
 * - Acquire an image from the swap chain
 * - Record a command buffer which draws the scene onto that image
 * - Submit the recorded command buffer
 * - Present the swap chain image
 */
void VulkanDevice::DrawFrame()
{
    vkWaitForFences(VdeviceQuery.GetChosenDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    vkResetFences(VdeviceQuery.GetChosenDevice(), 1, &inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(VdeviceQuery.GetChosenDevice(), swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffer, 0);

    RecordCommandBuffer(commandBuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(VdeviceQuery.GetGraphicsQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit Draw Command Buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; //Optional

    vkQueuePresentKHR(VdeviceQuery.GetPresentQueue(), &presentInfo);
}

void VulkanDevice::CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    if (vkCreateSemaphore(VdeviceQuery.GetChosenDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(VdeviceQuery.GetChosenDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(VdeviceQuery.GetChosenDevice(), &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Semaphores!");
    }
}