#pragma once

class Pipelines;
class CommandBuffer;
class RenderPass;
class SwapChain;
class VulkanSurface;
class DeviceQuery;
class VulkanInstance;
class Camera;
class VertexBuffer;

//This will hold pointers to all major Vulkan Objects
class VulkanCore
{
public:

    VulkanCore(const VulkanCore&) = delete;
    
    static VulkanCore& Get()
    {
        static VulkanCore s_Instance;
        return s_Instance;
    }
    
    static void SetDevice(DeviceQuery* device);
    static DeviceQuery* GetChosenDevice();

    static void SetInstance(VulkanInstance* instance);
    static VulkanInstance* GetInstance();

    static void SetSurface(VulkanSurface* surface);
    static VulkanSurface* GetSurface();

    static void SetSwapChain(SwapChain* swapChain);
    static SwapChain* GetSwapChain();

    static void SetRenderPass(RenderPass* renderPass);
    static RenderPass* GetRenderPass();

    static void SetCommandBuffer(CommandBuffer* commandBuffer);
    static CommandBuffer* GetCommandBuffer();

    static void SetPipeline(Pipelines* pipeline);
    static Pipelines* GetPipeline();

    static void SetCamera(Camera* camera);
    static Camera* GetCamera();

    static void SetVertexBuffer(VertexBuffer* vertexBuffer);
    static VertexBuffer* GetVertexBuffer();
    
private:

    VulkanCore() = default;
    ~VulkanCore();
    
    void ISetDevice(DeviceQuery* device);
    DeviceQuery* IGetChosenDevice() const;

    void ISetInstance(VulkanInstance* instance);
    VulkanInstance* IGetInstance() const;

    void ISetSurface(VulkanSurface* surface);
    VulkanSurface* IGetSurface() const;

    void ISetSwapChain(SwapChain* swapChain);
    SwapChain* IGetSwapChain() const;

    void ISetRenderPass(RenderPass* renderPass);
    RenderPass* IGetRenderPass() const;

    void ISetCommandBuffer(CommandBuffer* commandBuffer);
    CommandBuffer* IGetCommandBuffer();

    void ISetPipeline(Pipelines* pipeline);
    Pipelines* IGetPipeline();

    void ISetCamera(Camera* camera);
    Camera* IGetCamera();

    void ISetVertexBuffer(VertexBuffer* vertexBuffer);
    VertexBuffer* IGetVertexBuffer();
    
    DeviceQuery* VDevice = nullptr;
    VulkanInstance* VInstance = nullptr;
    VulkanSurface* VSurface = nullptr;
    SwapChain* VSwapChain = nullptr;
    RenderPass* VRenderPass = nullptr;
    CommandBuffer* VCommandBuffer = nullptr;
    Pipelines* VPipeline = nullptr;
    Camera* VCamera = nullptr;
    VertexBuffer* VVertexBuffer = nullptr;
};