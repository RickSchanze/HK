#pragma once

#include "Core/Container/Array.h"
#include "Core/String/String.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHIBuffer.h"
#include "RHI/RHICommand.h"
#include "RHI/RHICommandBuffer.h"
#include "RHI/RHICommandPool.h"
#include "RHI/RHIDescriptorSet.h"
#include "RHI/RHIImage.h"
#include "RHI/RHIImageView.h"
#include "RHI/RHIPipeline.h"
#include "RHI/RHISync.h"
#include <vulkan/vulkan.h>
#include "vulkan/vulkan.hpp"

class FGfxDeviceVk : public FGfxDevice
{
public:
    void Init() override;
    void UnInit() override;
    void WaitIdle() override;

#pragma region Buffer操作
    FRHIBuffer CreateBuffer(const FRHIBufferDesc& BufferCreateInfo) override;
    void DestroyBuffer(FRHIBuffer& Buffer) override;
    void* MapBuffer(FRHIBuffer& Buffer, UInt64 Offset, UInt64 Size) override;
    void UnmapBuffer(FRHIBuffer& Buffer) override;
#pragma endregion

#pragma region Image操作
    FRHIImage CreateImage(const FRHIImageDesc& ImageCreateInfo) override;
    void DestroyImage(FRHIImage& Image) override;
    FRHIImageView CreateImageView(const FRHIImage& Image, const FRHIImageViewDesc& ViewCreateInfo) override;
    void DestroyImageView(FRHIImageView& ImageView) override;
#pragma endregion

#pragma region Descriptor操作
    FRHIDescriptorSetLayout CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDesc& LayoutCreateInfo) override;
    void DestroyDescriptorSetLayout(FRHIDescriptorSetLayout& DescriptorSetLayout) override;
    FRHIDescriptorPool CreateDescriptorPool(const FRHIDescriptorPoolDesc& PoolCreateInfo) override;
    void DestroyDescriptorPool(FRHIDescriptorPool& DescriptorPool) override;
    FRHIDescriptorSet AllocateDescriptorSet(const FRHIDescriptorPool& Pool,
                                            const FRHIDescriptorSetDesc& SetCreateInfo) override;
    void FreeDescriptorSet(const FRHIDescriptorPool& Pool, FRHIDescriptorSet& DescriptorSet) override;
#pragma endregion

#pragma region Pipeline操作
    FRHIShaderModule CreateShaderModule(const FRHIShaderModuleDesc& ModuleCreateInfo, ERHIShaderStage Stage) override;
    void DestroyShaderModule(FRHIShaderModule& ShaderModule) override;
    FRHIPipelineLayout CreatePipelineLayout(const FRHIPipelineLayoutDesc& LayoutCreateInfo) override;
    void DestroyPipelineLayout(FRHIPipelineLayout& PipelineLayout) override;
    FRHIPipeline CreateGraphicsPipeline(const FRHIGraphicsPipelineDesc& PipelineCreateInfo) override;
    FRHIPipeline CreateComputePipeline(const FRHIComputePipelineDesc& PipelineCreateInfo) override;
    FRHIPipeline CreateRayTracingPipeline(const FRHIRayTracingPipelineDesc& PipelineCreateInfo) override;
    void DestroyPipeline(FRHIPipeline& Pipeline) override;
#pragma endregion

#pragma region Sync操作
    FRHISemaphore CreateSemaphore(const FRHISemaphoreDesc& SemaphoreCreateInfo) override;
    void DestroySemaphore(FRHISemaphore& Semaphore) override;
    FRHIFence CreateFence(const FRHIFenceDesc& FenceCreateInfo) override;
    void DestroyFence(FRHIFence& Fence) override;
    bool WaitForFence(const FRHIFence& Fence, UInt64 Timeout = std::numeric_limits<UInt64>::max()) override;
    bool IsFenceSignaled(const FRHIFence& Fence) const override;
    bool ResetFence(const FRHIFence& Fence) override;
#pragma endregion

#pragma region CommandPool操作
    FRHICommandPool CreateCommandPool(const FRHICommandPoolDesc& PoolCreateInfo) override;
    void DestroyCommandPool(FRHICommandPool& CommandPool) override;
#pragma endregion

#pragma region CommandBuffer操作
    FRHICommandBuffer CreateCommandBuffer(const FRHICommandPool& Pool,
                                           const FRHICommandBufferDesc& CommandBufferCreateInfo) override;
    void DestroyCommandBuffer(const FRHICommandPool& Pool, FRHICommandBuffer& CommandBuffer) override;
    void ExecuteCommand(FRHICommandBuffer& CommandBuffer, const FRHICommand& Command) override;

    bool SubmitCommandBuffer(FRHICommandBuffer& CommandBuffer, const TArray<FRHISemaphore>& WaitSemaphores,
                             const TArray<FRHISemaphore>& SignalSemaphores, const FRHIFence& Fence) override;
#pragma endregion

#pragma region 窗口操作
    void CreateMainWindowSurface(FName MainWindowName, FVector2i MainWindowInitSize,
                                 FRHIWindow& OutMainWindow) override;
    void CreateMainWindowSwapChain(FRHIWindow& OutMainWindow) override;
    void CreateRHIWindow(FName Name, FVector2i Size, FRHIWindow& OutWindow) override;
    void DestroyMainWindow(FRHIWindow& MainWindow) override;
    void DestroyRHIWindow(FRHIWindow& Window) override;
    void OpenWindow(FRHIWindow& Window) override;
    void CloseWindow(FRHIWindow& Window) override;
#pragma endregion

    VkDevice GetDevice() const
    {
        return Device;
    }

    vk::Instance GetInstance() const
    {
        return Instance;
    }

private:
    /**
     * 创建VkInstance
     */
    void CreateInstance();

    /**
     * 创建VkDevice
     */
    void CreateDevice();

    /**
     * 创建主窗口和Surface（内部辅助方法）
     * @param MainWindowName 主窗口名称
     * @param MainWindowInitSize 主窗口初始大小
     */
    void CreateMainWindowAndSurface(FName MainWindowName, FVector2i MainWindowInitSize);

    /**
     * 获取需要的Extension
     * @return
     */
    static TArray<const char*> GetRequiredExtensions();

    /**
     * 检查验证层的可用性
     * @param RequiredLayers
     * @return
     */
    static bool CheckValidationLayerSupport(const TArray<const char*>& RequiredLayers);

    /**
     * 获取验证层Layer名字
     * @return
     */
    static TArray<const char*> GetRequiredValidationLayers();

    /**
     * 选择物理设备
     */
    void SelectPhysicalDevice();

    /**
     * 获取需要的设备扩展
     * @return
     */
    static TArray<const char*> GetRequiredDeviceExtensions();

    /**
     * 检查设备是否适合
     * @param Device 物理设备
     * @return
     */
    bool IsDeviceSuitable(vk::PhysicalDevice Device) const;

    /**
     * 查找队列族
     * @param Device 物理设备
     * @return 队列族索引（图形队列、呈现队列等）
     */
    struct FQueueFamilyIndices
    {
        Int32 GraphicsFamily = -1;
        Int32 PresentFamily = -1;

        bool IsComplete() const
        {
            return GraphicsFamily >= 0 && PresentFamily >= 0;
        }
    };
    FQueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice Device) const;

    /**
     * 转换 EBufferUsage 到 VkBufferUsageFlags
     * @param Usage 缓冲区使用标志
     * @return Vulkan 缓冲区使用标志
     */
    static vk::BufferUsageFlags ConvertBufferUsage(ERHIBufferUsage Usage);

    /**
     * 转换 EBufferMemoryProperty 到 VkMemoryPropertyFlags
     * @param MemoryProperty 内存属性
     * @return Vulkan 内存属性标志
     */
    static vk::MemoryPropertyFlags ConvertMemoryProperty(ERHIBufferMemoryProperty MemoryProperty);

    /**
     * 转换 EImageFormat 到 VkFormat
     * @param Format 图像格式
     * @return Vulkan 图像格式
     */
    static vk::Format ConvertImageFormat(ERHIImageFormat Format);

    /**
     * 转换 EImageUsage 到 VkImageUsageFlags
     * @param Usage 图像使用标志
     * @return Vulkan 图像使用标志
     */
    static vk::ImageUsageFlags ConvertImageUsage(ERHIImageUsage Usage);

    /**
     * 转换 EImageLayout 到 VkImageLayout
     * @param Layout 图像布局
     * @return Vulkan 图像布局
     */
    static vk::ImageLayout ConvertImageLayout(ERHIImageLayout Layout);
    static vk::AccessFlags ConvertAccessFlags(ERHIAccessFlag AccessFlags);
    static vk::PipelineStageFlags ConvertPipelineStageFlags(ERHIPipelineStageFlag StageFlags);
    static vk::DependencyFlags ConvertDependencyFlags(ERHIDependencyFlag DependencyFlags);

    /**
     * 转换 EImageType 到 VkImageType
     * @param Type 图像类型
     * @return Vulkan 图像类型
     */
    static vk::ImageType ConvertImageType(ERHIImageType Type);

    /**
     * 转换 ESampleCount 到 VkSampleCountFlagBits
     * @param Samples 采样数量
     * @return Vulkan 采样计数标志
     */
    static vk::SampleCountFlagBits ConvertSampleCount(ERHISampleCount Samples);

    /**
     * 转换 ERHIDescriptorType 到 VkDescriptorType
     * @param Type 描述符类型
     * @return Vulkan 描述符类型
     */
    static vk::DescriptorType ConvertDescriptorType(ERHIDescriptorType Type);

    /**
     * 转换 ERHIDescriptorPoolCreateFlag 到 VkDescriptorPoolCreateFlags
     * @param Flags 描述符池创建标志
     * @return Vulkan 描述符池创建标志
     */
    static vk::DescriptorPoolCreateFlags ConvertDescriptorPoolCreateFlags(ERHIDescriptorPoolCreateFlag Flags);

    /**
     * 转换 ERHIShaderStage 到 VkShaderStageFlagBits
     * @param Stage 着色器阶段
     * @return Vulkan 着色器阶段标志
     */
    static vk::ShaderStageFlagBits ConvertShaderStage(ERHIShaderStage Stage);

    /**
     * 转换 ERHIShaderStage 到 VkShaderStageFlags
     * @param Stages 着色器阶段标志组合
     * @return Vulkan 着色器阶段标志
     */
    static vk::ShaderStageFlags ConvertShaderStageFlags(ERHIShaderStage Stages);

    /**
     * 转换 ERHIPrimitiveTopology 到 VkPrimitiveTopology
     */
    static vk::PrimitiveTopology ConvertPrimitiveTopology(ERHIPrimitiveTopology Topology);

    /**
     * 转换 ERHIPolygonMode 到 VkPolygonMode
     */
    static vk::PolygonMode ConvertPolygonMode(ERHIPolygonMode Mode);

    /**
     * 转换 ERHICullMode 到 VkCullModeFlags
     */
    static vk::CullModeFlags ConvertCullMode(ERHICullMode Mode);

    /**
     * 转换 ERHIFrontFace 到 VkFrontFace
     */
    static vk::FrontFace ConvertFrontFace(ERHIFrontFace Face);

    /**
     * 转换 ERHICompareOp 到 VkCompareOp
     */
    static vk::CompareOp ConvertCompareOp(ERHICompareOp Op);

    /**
     * 转换 ERHIStencilOp 到 VkStencilOp
     */
    static vk::StencilOp ConvertStencilOp(ERHIStencilOp Op);

    /**
     * 转换 ERHIBlendFactor 到 VkBlendFactor
     */
    static vk::BlendFactor ConvertBlendFactor(ERHIBlendFactor Factor);

    /**
     * 转换 ERHIBlendOp 到 VkBlendOp
     */
    static vk::BlendOp ConvertBlendOp(ERHIBlendOp Op);

    /**
     * 转换 ERHILogicOp 到 VkLogicOp
     */
    static vk::LogicOp ConvertLogicOp(ERHILogicOp Op);

    /**
     * 转换 ERHIDynamicState 标志位到 VkDynamicState 数组
     */
    static TArray<vk::DynamicState> ConvertDynamicStateFlags(ERHIDynamicState States);

    /**
     * 转换 ERHIColorComponentFlag 到 VkColorComponentFlags
     */
    static vk::ColorComponentFlags ConvertColorComponentFlags(ERHIColorComponentFlag Flags);

    /**
     * 内部辅助函数：创建图形管线（Vulkan 实现）
     */
    vk::Pipeline CreateGraphicsPipelineInternal(vk::PipelineLayout Layout, const FRHIGraphicsPipelineDesc& Desc) const;

    /**
     * 内部辅助函数：创建计算管线（Vulkan 实现）
     */
    vk::Pipeline CreateComputePipelineInternal(vk::PipelineLayout Layout, const FRHIComputePipelineDesc& Desc);

    /**
     * 查找合适的内存类型索引
     * @param TypeFilter 内存类型过滤器
     * @param Properties 所需的内存属性
     * @return 内存类型索引，失败返回 -1
     */
    UInt32 FindMemoryType(UInt32 TypeFilter, vk::MemoryPropertyFlags Properties) const;

    /**
     * 设置 Vulkan 对象的 DebugName
     * @param ObjectHandle 对象句柄
     * @param ObjectType 对象类型
     * @param Name 调试名称
     */
    void SetDebugName(vk::DeviceMemory ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::Buffer ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::Image ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::ImageView ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::DescriptorSetLayout ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::DescriptorPool ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::DescriptorSet ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::ShaderModule ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::PipelineLayout ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::Pipeline ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::Semaphore ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::Fence ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::CommandPool ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;
    void SetDebugName(vk::CommandBuffer ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const;

    /**
     * 转换命令缓冲区使用标志到 Vulkan 标志
     */
    static vk::CommandBufferUsageFlags ConvertCommandBufferUsageFlags(ERHICommandBufferUsageFlag Flags);

    vk::PhysicalDevice PhysicalDevice;
    vk::Device Device;
    vk::Instance Instance;
    vk::SurfaceKHR MainWindowSurface;
    vk::Queue GraphicsQueue;
    vk::Queue PresentQueue;
    FQueueFamilyIndices QueueFamilyIndices;
    bool bValidationLayersEnabled = false;
    bool bDebugUtilsExtensionAvailable = false;                              // Debug Utils扩展是否可用
    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = nullptr; // 缓存的Debug Utils函数指针
};
