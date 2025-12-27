#pragma once

#include "Core/Event/Event.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Utility/Macros.h"
#include "Math/Vector.h"
#include "RHIPipeline.h"
#include "RHISync.h"

#include "GfxDevice.generated.h"

class FRHIWindow;
struct FRHIBufferDesc;
class FRHIBuffer;
struct FRHIImageDesc;
class FRHIImage;
struct FRHIImageViewDesc;
class FRHIImageView;
struct FRHIDescriptorPoolDesc;
class FRHIDescriptorPool;
struct FRHIDescriptorSetLayoutDesc;
class FRHIDescriptorSetLayout;
struct FRHIDescriptorSetDesc;
class FRHIDescriptorSet;
struct FRHIShaderModuleDesc;
class FRHIShaderModule;
struct FRHIPipelineLayoutDesc;
class FRHIPipelineLayout;
struct FRHIGraphicsPipelineDesc;
struct FRHIComputePipelineDesc;
struct FRHIRayTracingPipelineDesc;
class FRHIPipeline;
struct FRHICommandPoolDesc;
class FRHICommandPool;
struct FRHICommandBufferDesc;
class FRHICommandBuffer;
struct FRHICommand;

HENUM()
enum class EGfxBackend
{
    Vulkan,
    D3D12,
    Metal,
    GL,
    GLES,
    Count,
};

class HK_API FGfxDevice
{
public:
    virtual void Init()   = 0;
    virtual void UnInit() = 0;

    virtual ~FGfxDevice() = default;

#pragma region Buffer操作
    // 创建缓冲区，返回的值类型包含一个 Handle
    // 可以像普通值类型一样拷贝和移动
    // 只有通过 CreateBuffer 创建的 Buffer 才是有效的
    virtual FRHIBuffer CreateBuffer(const FRHIBufferDesc& BufferCreateInfo) = 0;

    // 销毁缓冲区资源
    // 必须通过此方法销毁，不能直接调用 Buffer.Destroy()
    virtual void DestroyBuffer(FRHIBuffer& Buffer) = 0;

    // 映射缓冲区内存（用于 CPU 访问）
    // @param Buffer 要映射的缓冲区
    // @param Offset 映射的偏移量（字节）
    // @param Size 映射的大小（字节），0 表示映射到缓冲区末尾
    // @return 映射的内存指针
    virtual void* MapBuffer(FRHIBuffer& Buffer, UInt64 Offset, UInt64 Size) = 0;

    // 取消映射缓冲区内存
    // @param Buffer 要取消映射的缓冲区
    virtual void UnmapBuffer(FRHIBuffer& Buffer) = 0;
#pragma endregion

#pragma region Image操作
    // 创建图像，返回的值类型包含一个 Handle
    // 可以像普通值类型一样拷贝和移动
    // 只有通过 CreateImage 创建的 Image 才是有效的
    virtual FRHIImage CreateImage(const FRHIImageDesc& ImageCreateInfo) = 0;

    // 销毁图像资源
    // 必须通过此方法销毁，不能直接调用 Image.Destroy()
    virtual void DestroyImage(FRHIImage& Image) = 0;

    // 创建图像视图
    // 图像视图允许以不同的方式访问图像的子资源
    virtual FRHIImageView CreateImageView(const FRHIImage& Image, const FRHIImageViewDesc& ViewCreateInfo) = 0;

    // 销毁图像视图资源
    // 必须通过此方法销毁，不能直接调用 ImageView.Destroy()
    virtual void DestroyImageView(FRHIImageView& ImageView) = 0;
#pragma endregion

#pragma region Descriptor操作
    // 创建描述符集布局
    // 描述符集布局定义了描述符集的绑定信息
    // @param LayoutCreateInfo 描述符集布局创建信息
    // @return 创建的描述符集布局
    virtual FRHIDescriptorSetLayout CreateDescriptorSetLayout(const FRHIDescriptorSetLayoutDesc& LayoutCreateInfo) = 0;

    // 销毁描述符集布局资源
    // 必须通过此方法销毁，不能直接调用 DescriptorSetLayout.Destroy()
    virtual void DestroyDescriptorSetLayout(FRHIDescriptorSetLayout& DescriptorSetLayout) = 0;

    // 创建描述符池
    // 描述符池用于分配描述符集
    virtual FRHIDescriptorPool CreateDescriptorPool(const FRHIDescriptorPoolDesc& PoolCreateInfo) = 0;

    // 销毁描述符池资源
    // 销毁描述符池会自动释放所有从中分配的描述符集
    // 必须通过此方法销毁，不能直接调用 DescriptorPool.Destroy()
    virtual void DestroyDescriptorPool(FRHIDescriptorPool& DescriptorPool) = 0;

    // 从描述符池分配描述符集
    // @param Pool 描述符池
    // @param SetCreateInfo 描述符集创建信息
    // @return 分配的描述符集
    virtual FRHIDescriptorSet AllocateDescriptorSet(const FRHIDescriptorPool&    Pool,
                                                    const FRHIDescriptorSetDesc& SetCreateInfo) = 0;

    // 释放描述符集
    // 将描述符集返回到描述符池
    // @param Pool 描述符池
    // @param DescriptorSet 要释放的描述符集
    virtual void FreeDescriptorSet(const FRHIDescriptorPool& Pool, FRHIDescriptorSet& DescriptorSet) = 0;
#pragma endregion

#pragma region Pipeline操作
    // 创建着色器模块
    // @param ModuleCreateInfo 着色器模块创建信息
    // @param Stage 着色器阶段
    // @return 创建的着色器模块
    virtual FRHIShaderModule CreateShaderModule(const FRHIShaderModuleDesc& ModuleCreateInfo,
                                                ERHIShaderStage             Stage) = 0;

    // 销毁着色器模块资源
    // 必须通过此方法销毁，不能直接调用 ShaderModule.Destroy()
    virtual void DestroyShaderModule(FRHIShaderModule& ShaderModule) = 0;

    // 创建管线布局
    // 管线布局定义了描述符集布局和推送常量的组合
    // @param LayoutCreateInfo 管线布局创建信息
    // @return 创建的管线布局
    virtual FRHIPipelineLayout CreatePipelineLayout(const FRHIPipelineLayoutDesc& LayoutCreateInfo) = 0;

    // 销毁管线布局资源
    // 必须通过此方法销毁，不能直接调用 PipelineLayout.Destroy()
    virtual void DestroyPipelineLayout(FRHIPipelineLayout& PipelineLayout) = 0;

    // 创建图形管线
    // @param PipelineCreateInfo 图形管线创建信息
    // @return 创建的管线
    virtual FRHIPipeline CreateGraphicsPipeline(const FRHIGraphicsPipelineDesc& PipelineCreateInfo) = 0;

    // 创建计算管线
    // @param PipelineCreateInfo 计算管线创建信息
    // @return 创建的管线
    virtual FRHIPipeline CreateComputePipeline(const FRHIComputePipelineDesc& PipelineCreateInfo) = 0;

    // 创建光线追踪管线
    // @param PipelineCreateInfo 光线追踪管线创建信息
    // @return 创建的管线
    virtual FRHIPipeline CreateRayTracingPipeline(const FRHIRayTracingPipelineDesc& PipelineCreateInfo) = 0;

    // 销毁管线资源
    // 必须通过此方法销毁，不能直接调用 Pipeline.Destroy()
    virtual void DestroyPipeline(FRHIPipeline& Pipeline) = 0;
#pragma endregion

#pragma region Sync操作
    // 创建信号量
    // 信号量用于GPU命令之间的同步，通常用于队列提交之间的同步
    // @param SemaphoreCreateInfo 信号量创建信息
    // @return 创建的信号量
    virtual FRHISemaphore CreateSemaphore(const FRHISemaphoreDesc& SemaphoreCreateInfo) = 0;

    // 销毁信号量资源
    // 必须通过此方法销毁，不能直接调用 Semaphore.Destroy()
    virtual void DestroySemaphore(FRHISemaphore& Semaphore) = 0;

    // 创建栅栏
    // 栅栏用于CPU和GPU之间的同步，可以等待GPU完成某些操作
    // @param FenceCreateInfo 栅栏创建信息
    // @return 创建的栅栏
    virtual FRHIFence CreateFence(const FRHIFenceDesc& FenceCreateInfo) = 0;

    // 销毁栅栏资源
    // 必须通过此方法销毁，不能直接调用 Fence.Destroy()
    virtual void DestroyFence(FRHIFence& Fence) = 0;
#pragma endregion

#pragma region CommandPool操作
    // 创建命令池
    // 命令池用于分配命令缓冲区
    // @param PoolCreateInfo 命令池创建信息
    // @return 创建的命令池
    virtual FRHICommandPool CreateCommandPool(const FRHICommandPoolDesc& PoolCreateInfo) = 0;

    // 销毁命令池资源
    // 销毁命令池会自动释放所有从中分配的命令缓冲区
    // 必须通过此方法销毁，不能直接调用 CommandPool.Destroy()
    // @param CommandPool 要销毁的命令池
    virtual void DestroyCommandPool(FRHICommandPool& CommandPool) = 0;
#pragma endregion

#pragma region CommandBuffer操作
    // 创建命令缓冲区，返回的值类型包含一个 Handle
    // 可以像普通值类型一样拷贝和移动
    // 只有通过 CreateCommandBuffer 创建的 CommandBuffer 才是有效的
    // @param Pool 命令池（必须有效）
    // @param CommandBufferCreateInfo 命令缓冲区创建信息
    // @return 创建的命令缓冲区
    virtual FRHICommandBuffer CreateCommandBuffer(const FRHICommandPool&       Pool,
                                                  const FRHICommandBufferDesc& CommandBufferCreateInfo) = 0;

    // 销毁命令缓冲区资源
    // 必须通过此方法销毁，不能直接调用 CommandBuffer.Destroy()
    // @param Pool 命令池（必须有效）
    // @param CommandBuffer 要销毁的命令缓冲区
    virtual void DestroyCommandBuffer(const FRHICommandPool& Pool, FRHICommandBuffer& CommandBuffer) = 0;

    // 执行命令（内部方法，由 FRHICommandBuffer 调用）
    // @param CommandBuffer 命令缓冲区
    // @param Command 要执行的命令
    virtual void ExecuteCommand(FRHICommandBuffer& CommandBuffer, const FRHICommand& Command) = 0;
#pragma endregion

#pragma region "窗口操作"
    // 窗口创建相关的函数, 这块的流程是这样的
    // 以Vulkan为例, Vulkan初始化Instance -> 拿Instance初始化MainWindow
    // 为MainWindow创建Surface -> Vulkan利用此Surface创建Device
    // 利用Device创建MainWindow的SwapChain
    // 销毁时先销毁SwapChain再销毁Surface, 这一点主窗口和其他一致
    /**
     * 创建主窗口的Surface
     * @param MainWindowName
     * @param MainWindowInitSize
     * @param OutMainWindow
     */
    virtual void CreateMainWindowSurface(FName MainWindowName, FVector2i MainWindowInitSize,
                                         FRHIWindow& OutMainWindow) = 0;

    /**
     * 创建主窗口的SwapChain
     * @param OutMainWindow
     */
    virtual void CreateMainWindowSwapChain(FRHIWindow& OutMainWindow) = 0;

    /**
     * 创建一个窗口
     * @param Name
     * @param Size
     * @param OutWindow
     */
    virtual void CreateRHIWindow(FName Name, FVector2i Size, FRHIWindow& OutWindow) = 0;

    /**
     * 销毁主窗口
     * @param MainWindow
     */
    virtual void DestroyMainWindow(FRHIWindow& MainWindow) = 0;

    /**
     * 销毁一个窗口
     * @param Window
     */
    virtual void DestroyRHIWindow(FRHIWindow& Window) = 0;

    /**
     * 打开窗口（显示窗口）
     * @param Window
     */
    virtual void OpenWindow(FRHIWindow& Window) = 0;

    /**
     * 关闭窗口（隐藏窗口）
     * @param Window
     */
    virtual void CloseWindow(FRHIWindow& Window) = 0;
#pragma endregion
};

inline TEvent<>            GOnPreRHIDeviceCreated;
inline TEvent<FGfxDevice*> GOnPostRHIDeviceCreated;
inline TEvent<FGfxDevice*> GOnPreRHIDeviceDestroyed;
inline TEvent<>            GOnPostRHIDeviceDestroyed;

HK_API void CreateGfxDevice();
HK_API void DestroyGfxDevice();

HK_API FGfxDevice* GetGfxDevice();
HK_API FGfxDevice& GetGfxDeviceRef();
