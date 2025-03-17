#include "Pipelines.h"

#include <iostream>

#include "DeviceQuery.h"
#include "RenderPass.h"

Pipelines::Pipelines()
{
    graphicsPipeline = VK_NULL_HANDLE;
    pipelineLayout = VK_NULL_HANDLE;
}

Pipelines::~Pipelines()
{
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
void Pipelines::CreateGraphicsPipeline(DeviceQuery& deviceQuery, RenderPass& renderPass)
{
    auto vertShaderCode = ReadFile("shaders/vert.spv");
    auto fragShaderCode = ReadFile("shaders/frag.spv");

    VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode, deviceQuery);
    VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode, deviceQuery);

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
    
    VkVertexInputBindingDescription bindingDescription = Vertex::GetBindingDescription();
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = Vertex::GetAttributeDescriptions();
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

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

    if (vkCreatePipelineLayout(deviceQuery.GetChosenDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
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
    pipelineInfo.renderPass = renderPass.GetRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; //Optional
    pipelineInfo.basePipelineIndex = -1; //Optional

    if (vkCreateGraphicsPipelines(deviceQuery.GetChosenDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a Graphics Pipeline!");
    }
    
    vkDestroyShaderModule(deviceQuery.GetChosenDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(deviceQuery.GetChosenDevice(), vertShaderModule, nullptr);
}

void Pipelines::CreateVertexBuffer(DeviceQuery& device)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.GetChosenDevice(), &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to Create Vertex Buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.GetChosenDevice(), vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, device);

    if (vkAllocateMemory(device.GetChosenDevice(), &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocated Vertex Buffer Memory!");
    }

    vkBindBufferMemory(device.GetChosenDevice(), vertexBuffer, vertexBufferMemory, 0);

    void* data;
    vkMapMemory(device.GetChosenDevice(), vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(device.GetChosenDevice(), vertexBufferMemory);
}

VkPipelineLayout& Pipelines::GetPipelineLayout()
{
    return pipelineLayout;
}

VkPipeline& Pipelines::GetGraphicsPipeline()
{
    return graphicsPipeline;
}

VkBuffer& Pipelines::GetVertexBuffer()
{
    return vertexBuffer;
}

void Pipelines::CleanUp(DeviceQuery& device)
{
    vkDestroyBuffer(device.GetChosenDevice(), vertexBuffer, nullptr);

    vkFreeMemory(device.GetChosenDevice(), vertexBufferMemory, nullptr);

    vkDestroyPipeline(device.GetChosenDevice(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device.GetChosenDevice(), pipelineLayout, nullptr);
}

std::vector<char> Pipelines::ReadFile(const std::string& fileName)
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

VkShaderModule Pipelines::CreateShaderModule(const std::vector<char>& code, DeviceQuery& deviceQury)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(deviceQury.GetChosenDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Shader Module!");
    }

    return shaderModule;
}

uint32_t Pipelines::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, DeviceQuery& device)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device.GetPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("Failed to find a suitable Memory Type!");
    
    return 0;
}