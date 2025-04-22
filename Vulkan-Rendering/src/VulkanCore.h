#pragma once
#include <array>
#include <vector>

class ModelLoader;
class Pipelines;
class CommandBuffer;
class RenderPass;
class SwapChain;
class VulkanSurface;
class DeviceQuery;
class VulkanInstance;
class Camera;

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

    static void SetModels(std::vector<ModelLoader>* models);
    static std::vector<ModelLoader>* GetModels();
    
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
    CommandBuffer* IGetCommandBuffer() const;

    void ISetPipeline(Pipelines* pipeline);
    Pipelines* IGetPipeline() const;

    void ISetCamera(Camera* camera);
    Camera* IGetCamera() const;

    void ISetModels(std::vector<ModelLoader>* models);
    std::vector<ModelLoader>* IGetModels() const;
    
    DeviceQuery* VDevice = nullptr;
    VulkanInstance* VInstance = nullptr;
    VulkanSurface* VSurface = nullptr;
    SwapChain* VSwapChain = nullptr;
    RenderPass* VRenderPass = nullptr;
    CommandBuffer* VCommandBuffer = nullptr;
    Pipelines* VPipeline = nullptr;
    Camera* VCamera = nullptr;

    std::vector<ModelLoader>* m_models = nullptr;
};