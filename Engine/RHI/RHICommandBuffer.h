#pragma once

#include "Core/Container/Array.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "Core/Utility/UniquePtr.h"
#include "Core/Utility/HashUtility.h"
#include "Math/Rect2D.h"
#include "Math/Vector.h"
#include "RHIBuffer.h"
#include "RHICommand.h"
#include "RHIDescriptorSet.h"
#include "RHIHandle.h"
#include "RHIImage.h"
#include "RHIPipeline.h"
#include "RHISync.h"

enum class ERHICommandExecuteMode
{
    Immediate,
    Deferred,
    Threaded,
};

// 命令缓冲区级别
enum class ERHICommandBufferLevel : UInt32
{
    Primary   = 0, // 主命令缓冲区（可以直接提交到队列）
    Secondary = 1, // 辅助命令缓冲区（只能从主命令缓冲区执行）
};

// ERHICommandBufferUsageFlag 定义在 RHICommand.h 中（避免循环依赖）

// 访问标志（对应 Vulkan 的 VkAccessFlags）
enum class ERHIAccessFlag : UInt32
{
    None                        = 0,
    IndirectRead                = 1 << 0,  // VK_ACCESS_INDIRECT_COMMAND_READ_BIT
    IndexRead                   = 1 << 1,  // VK_ACCESS_INDEX_READ_BIT
    VertexAttributeRead         = 1 << 2,  // VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
    UniformRead                 = 1 << 3,  // VK_ACCESS_UNIFORM_READ_BIT
    InputAttachmentRead         = 1 << 4,  // VK_ACCESS_INPUT_ATTACHMENT_READ_BIT
    ShaderRead                  = 1 << 5,  // VK_ACCESS_SHADER_READ_BIT
    ShaderWrite                 = 1 << 6,  // VK_ACCESS_SHADER_WRITE_BIT
    ColorAttachmentRead         = 1 << 7,  // VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
    ColorAttachmentWrite        = 1 << 8,  // VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    DepthStencilAttachmentRead  = 1 << 9,  // VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
    DepthStencilAttachmentWrite = 1 << 10, // VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    TransferRead                = 1 << 11, // VK_ACCESS_TRANSFER_READ_BIT
    TransferWrite               = 1 << 12, // VK_ACCESS_TRANSFER_WRITE_BIT
    HostRead                    = 1 << 13, // VK_ACCESS_HOST_READ_BIT
    HostWrite                   = 1 << 14, // VK_ACCESS_HOST_WRITE_BIT
    MemoryRead                  = 1 << 15, // VK_ACCESS_MEMORY_READ_BIT
    MemoryWrite                 = 1 << 16, // VK_ACCESS_MEMORY_WRITE_BIT
};
HK_ENABLE_BITMASK_OPERATORS(ERHIAccessFlag)

// 管线阶段标志（对应 Vulkan 的 VkPipelineStageFlags）
enum class ERHIPipelineStageFlag : UInt32
{
    None                         = 0,
    TopOfPipe                    = 1 << 0,  // VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
    DrawIndirect                 = 1 << 1,  // VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT
    VertexInput                  = 1 << 2,  // VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
    VertexShader                 = 1 << 3,  // VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
    TessellationControlShader    = 1 << 4,  // VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT
    TessellationEvaluationShader = 1 << 5,  // VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT
    GeometryShader               = 1 << 6,  // VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT
    FragmentShader               = 1 << 7,  // VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
    EarlyFragmentTests           = 1 << 8,  // VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
    LateFragmentTests            = 1 << 9,  // VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
    ColorAttachmentOutput        = 1 << 10, // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    ComputeShader                = 1 << 11, // VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
    Transfer                     = 1 << 12, // VK_PIPELINE_STAGE_TRANSFER_BIT
    BottomOfPipe                 = 1 << 13, // VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
    Host                         = 1 << 14, // VK_PIPELINE_STAGE_HOST_BIT
    AllGraphics                  = 1 << 15, // VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT
    AllCommands                  = 1 << 16, // VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
};
HK_ENABLE_BITMASK_OPERATORS(ERHIPipelineStageFlag)

// 依赖标志（对应 Vulkan 的 VkDependencyFlags）
enum class ERHIDependencyFlag : UInt32
{
    None        = 0,
    ByRegion    = 1 << 0, // VK_DEPENDENCY_BY_REGION_BIT
    DeviceGroup = 1 << 1, // VK_DEPENDENCY_DEVICE_GROUP_BIT
    ViewLocal   = 1 << 2, // VK_DEPENDENCY_VIEW_LOCAL_BIT
};
HK_ENABLE_BITMASK_OPERATORS(ERHIDependencyFlag)

// 命令缓冲区描述
struct FRHICommandBufferDesc
{
    ERHICommandBufferLevel     Level      = ERHICommandBufferLevel::Primary;  // 命令缓冲区级别
    ERHICommandBufferUsageFlag UsageFlags = ERHICommandBufferUsageFlag::None; // 使用标志
    FString                    DebugName;                                     // 调试名称

    UInt64 GetHashCode() const
    {
        return FHashUtility::CombineHashes(std::hash<UInt32>{}(static_cast<UInt32>(Level)),
                                           std::hash<UInt32>{}(static_cast<UInt32>(UsageFlags)));
    }
};

// 命令缓冲区类
class FRHICommandBuffer
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 CommandBuffer（无效）
    FRHICommandBuffer() = default;

    // 析构函数：不自动销毁资源，必须通过 FGfxDevice::DestroyCommandBuffer 销毁
    ~FRHICommandBuffer() = default;

    // [Fix] 禁用拷贝（因为包含 UniquePtr），允许移动
    FRHICommandBuffer(const FRHICommandBuffer& Other)                = delete;
    FRHICommandBuffer& operator=(const FRHICommandBuffer& Other)     = delete;
    FRHICommandBuffer(FRHICommandBuffer&& Other) noexcept            = default;
    FRHICommandBuffer& operator=(FRHICommandBuffer&& Other) noexcept = default;

    // 检查是否有效
    bool IsValid() const
    {
        return Handle.IsValid();
    }

    // 获取底层句柄
    const FRHIHandle& GetHandle() const
    {
        return Handle;
    }

    FRHIHandle& GetHandle()
    {
        return Handle;
    }

    // 获取命令缓冲区级别
    ERHICommandBufferLevel GetLevel() const
    {
        return Level;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHICommandBuffer& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHICommandBuffer& Other) const
    {
        return Handle != Other.Handle;
    }

    UInt64 GetHashCode() const
    {
        return Handle.GetHashCode();
    }

    // 获取执行模式
    ERHICommandExecuteMode GetExecuteMode() const
    {
        return ExecuteMode;
    }

    // 设置执行模式
    void SetExecuteMode(ERHICommandExecuteMode InExecuteMode)
    {
        ExecuteMode = InExecuteMode;
    }

    // 执行所有排队的命令
    // Immediate 模式：什么也不做（命令已经立即执行）
    // Deferred 模式：在本线程执行所有排队的命令
    // Threaded 模式：将命令提交给渲染线程（暂时忽略）
    void Execute();

    // 提交命令缓冲区到 GPU 队列
    // @param WaitSemaphores 等待的信号量数组（可选）
    // @param SignalSemaphores 信号信号量数组（可选）
    // @param Fence 栅栏（可选，用于等待提交完成）
    // @return 是否提交成功
    bool Submit(const TArray<FRHISemaphore>& WaitSemaphores = {},
                const TArray<FRHISemaphore>& SignalSemaphores = {},
                const FRHIFence& Fence = FRHIFence());

    // 获取命令队列大小
    UInt32 GetCommandCount() const
    {
        return static_cast<UInt32>(CommandQueue.Size());
    }

    // 清空命令队列
    void ClearCommands();

#pragma region 命令缓冲区生命周期
    // 开始记录命令
    // @param UsageFlags 使用标志（可选，覆盖创建时的标志）
    void Begin(ERHICommandBufferUsageFlag UsageFlags = ERHICommandBufferUsageFlag::None);

    // 结束记录命令
    void End();

    // 重置命令缓冲区
    // @param ReleaseResources 是否释放资源
    void Reset(bool ReleaseResources = false);
#pragma endregion

#pragma region 管线绑定
    // 绑定图形管线
    // @param Pipeline 要绑定的图形管线
    void BindPipeline(const FRHIPipeline& Pipeline);

    // 绑定计算管线
    // @param Pipeline 要绑定的计算管线
    void BindComputePipeline(const FRHIPipeline& Pipeline);
#pragma endregion

#pragma region 描述符集绑定
    // 绑定描述符集
    // @param PipelineType 管线类型
    // @param Layout 管线布局
    // @param DescriptorSet 描述符集
    // @param FirstSet 第一个描述符集索引
    void BindDescriptorSet(ERHIPipelineType PipelineType, const FRHIPipelineLayout& Layout,
                           const FRHIDescriptorSet& DescriptorSet, UInt32 FirstSet = 0);

    // 绑定多个描述符集
    // @param PipelineType 管线类型
    // @param Layout 管线布局
    // @param DescriptorSets 描述符集数组
    // @param FirstSet 第一个描述符集索引
    void BindDescriptorSets(ERHIPipelineType PipelineType, const FRHIPipelineLayout& Layout,
                            const TArray<FRHIDescriptorSet>& DescriptorSets, UInt32 FirstSet = 0);
#pragma endregion

#pragma region 顶点和索引缓冲区绑定
    // 绑定顶点缓冲区
    // @param Binding 绑定索引
    // @param Buffer 顶点缓冲区
    // @param Offset 偏移量（字节）
    void BindVertexBuffer(UInt32 Binding, const FRHIBuffer& Buffer, UInt64 Offset = 0);

    // 绑定多个顶点缓冲区
    // @param FirstBinding 第一个绑定索引
    // @param Buffers 顶点缓冲区数组
    // @param Offsets 偏移量数组（字节）
    void BindVertexBuffers(UInt32 FirstBinding, const TArray<FRHIBuffer>& Buffers, const TArray<UInt64>& Offsets);

    // 绑定索引缓冲区
    // @param Buffer 索引缓冲区
    // @param Offset 偏移量（字节）
    // @param IndexType 索引类型（UInt16 或 UInt32）
    void BindIndexBuffer(const FRHIBuffer& Buffer, UInt64 Offset = 0, bool bIs32Bit = false);
#pragma endregion

#pragma region 绘制命令
    // 绘制（非索引）
    // @param VertexCount 顶点数量
    // @param InstanceCount 实例数量
    // @param FirstVertex 第一个顶点索引
    // @param FirstInstance 第一个实例索引
    void Draw(UInt32 VertexCount, UInt32 InstanceCount = 1, UInt32 FirstVertex = 0, UInt32 FirstInstance = 0);

    // 绘制索引
    // @param IndexCount 索引数量
    // @param InstanceCount 实例数量
    // @param FirstIndex 第一个索引索引
    // @param VertexOffset 顶点偏移量
    // @param FirstInstance 第一个实例索引
    void DrawIndexed(UInt32 IndexCount, UInt32 InstanceCount = 1, UInt32 FirstIndex = 0, Int32 VertexOffset = 0,
                     UInt32 FirstInstance = 0);

    // 间接绘制（非索引）
    // @param Buffer 间接绘制命令缓冲区
    // @param Offset 偏移量（字节）
    // @param DrawCount 绘制命令数量
    // @param Stride 命令之间的步长（字节）
    void DrawIndirect(const FRHIBuffer& Buffer, UInt64 Offset, UInt32 DrawCount = 1, UInt32 Stride = 0);

    // 间接绘制索引
    // @param Buffer 间接绘制命令缓冲区
    // @param Offset 偏移量（字节）
    // @param DrawCount 绘制命令数量
    // @param Stride 命令之间的步长（字节）
    void DrawIndexedIndirect(const FRHIBuffer& Buffer, UInt64 Offset, UInt32 DrawCount = 1, UInt32 Stride = 0);
#pragma endregion

#pragma region 计算调度命令
    // 调度计算着色器
    // @param GroupCountX X方向的组数量
    // @param GroupCountY Y方向的组数量
    // @param GroupCountZ Z方向的组数量
    void Dispatch(UInt32 GroupCountX, UInt32 GroupCountY = 1, UInt32 GroupCountZ = 1);

    // 间接调度计算着色器
    // @param Buffer 间接调度命令缓冲区
    // @param Offset 偏移量（字节）
    void DispatchIndirect(const FRHIBuffer& Buffer, UInt64 Offset);
#pragma endregion

#pragma region 缓冲区复制命令
    // 复制缓冲区
    // @param SrcBuffer 源缓冲区
    // @param DstBuffer 目标缓冲区
    // @param RegionCount 区域数量
    // @param Regions 复制区域数组（每个区域包含：srcOffset, dstOffset, size）
    void CopyBuffer(const FRHIBuffer& SrcBuffer, const FRHIBuffer& DstBuffer,
                    const TArray<FRHIBufferCopyRegion>& Regions);

    // 复制图像
    // @param SrcImage 源图像
    // @param DstImage 目标图像
    // @param RegionCount 区域数量
    // @param Regions 复制区域数组
    void CopyImage(const FRHIImage& SrcImage, const FRHIImage& DstImage, const TArray<FRHIImageCopyRegion>& Regions);

    // 从缓冲区复制到图像
    // @param SrcBuffer 源缓冲区
    // @param DstImage 目标图像
    // @param Regions 复制区域数组
    void CopyBufferToImage(const FRHIBuffer& SrcBuffer, const FRHIImage& DstImage,
                           const TArray<FRHIBufferImageCopyRegion>& Regions);

    // 从图像复制到缓冲区
    // @param SrcImage 源图像
    // @param DstBuffer 目标缓冲区
    // @param Regions 复制区域数组
    void CopyImageToBuffer(const FRHIImage& SrcImage, const FRHIBuffer& DstBuffer,
                           const TArray<FRHIBufferImageCopyRegion>& Regions);
#pragma endregion

#pragma region 图像清除命令
    // 清除颜色图像
    // @param Image 要清除的图像
    // @param Color 清除颜色
    // @param Ranges 清除范围数组
    void ClearColorImage(const FRHIImage& Image, const FVector4f& Color,
                         const TArray<FRHIImageSubresourceRange>& Ranges);

    // 清除深度模板图像
    // @param Image 要清除的图像
    // @param Depth 深度值
    // @param Stencil 模板值
    // @param Ranges 清除范围数组
    void ClearDepthStencilImage(const FRHIImage& Image, float Depth, UInt32 Stencil,
                                const TArray<FRHIImageSubresourceRange>& Ranges);
#pragma endregion

#pragma region 管线屏障和同步
    // 管线屏障
    // @param SrcStageMask 源阶段掩码
    // @param DstStageMask 目标阶段掩码
    // @param DependencyFlags 依赖标志
    // @param MemoryBarriers 内存屏障数组
    // @param BufferMemoryBarriers 缓冲区内存屏障数组
    // @param ImageMemoryBarriers 图像内存屏障数组
    void PipelineBarrier(ERHIPipelineStageFlag SrcStageMask, ERHIPipelineStageFlag DstStageMask,
                         ERHIDependencyFlag DependencyFlags, const TArray<FRHIMemoryBarrier>& MemoryBarriers,
                         const TArray<FRHIBufferMemoryBarrier>& BufferMemoryBarriers,
                         const TArray<FRHIImageMemoryBarrier>&  ImageMemoryBarriers);
#pragma endregion

#pragma region 视口和裁剪
    // 设置视口
    // @param FirstViewport 第一个视口索引
    // @param Viewports 视口数组
    void SetViewport(UInt32 FirstViewport, const TArray<FRHIViewport>& Viewports);

    // 设置裁剪矩形
    // @param FirstScissor 第一个裁剪矩形索引
    // @param Scissors 裁剪矩形数组
    void SetScissor(UInt32 FirstScissor, const TArray<FRHIRect2D>& Scissors);
#pragma endregion

#pragma region 推送常量
    // 推送常量
    // @param Layout 管线布局
    // @param StageFlags 着色器阶段标志
    // @param Offset 偏移量（字节）
    // @param Size 大小（字节）
    // @param Data 数据指针
    void PushConstants(const FRHIPipelineLayout& Layout, UInt32 StageFlags, UInt32 Offset, UInt32 Size,
                       const void* Data);
#pragma endregion

#pragma region 渲染通道（如果支持）
    // 开始渲染通道（已弃用，使用 BeginRendering 代替）
    // @param RenderPassBeginInfo 渲染通道开始信息
    // @param Contents 辅助命令缓冲区内容类型
    void BeginRenderPass(const FRHIRenderPassBeginInfo& RenderPassBeginInfo,
                         ERHICommandBufferLevel         Contents = ERHICommandBufferLevel::Primary);

    // 结束渲染通道（已弃用，使用 EndRendering 代替）
    void EndRenderPass();
#pragma endregion

#pragma region Dynamic Rendering
    // 开始动态渲染（推荐使用）
    // @param RenderTarget 渲染目标
    void BeginRendering(const class FRenderTarget& RenderTarget);

    // 结束动态渲染
    void EndRendering();
#pragma endregion

private:
    // 执行命令（内部方法，由 GfxDevice 实现调用）
    void ExecuteCommand(const FRHICommand& Command);

    // 添加命令到队列（Deferred 模式）或立即执行（Immediate 模式）
    void AddOrExecuteCommand(TUniquePtr<FRHICommand> Command);

    FRHIHandle                      Handle;
    ERHICommandBufferLevel          Level       = ERHICommandBufferLevel::Primary;
    ERHICommandExecuteMode          ExecuteMode = ERHICommandExecuteMode::Deferred; // 执行模式
    TArray<TUniquePtr<FRHICommand>> CommandQueue;         // 命令队列（Deferred/Threaded 模式）
    bool                            bIsRecording = false; // 是否正在记录命令
};

// 辅助结构定义（在类外部定义，供全局使用）
struct FRHIBufferCopyRegion
{
    UInt64 SrcOffset = 0; // 源偏移量（字节）
    UInt64 DstOffset = 0; // 目标偏移量（字节）
    UInt64 Size      = 0; // 复制大小（字节）
};

struct FRHIImageSubresourceLayers
{
    ERHIImageAspect AspectMask     = ERHIImageAspect::Color; // 方面掩码
    UInt32          MipLevel       = 0;                      // MIP级别
    UInt32          BaseArrayLayer = 0;                      // 基础数组层
    UInt32          LayerCount     = 1;                      // 层数量
};

struct FRHIImageCopyRegion
{
    FRHIImageSubresourceLayers SrcSubresource;        // 源子资源层
    FVector3i                  SrcOffset = {0, 0, 0}; // 源偏移量
    FRHIImageSubresourceLayers DstSubresource;        // 目标子资源层
    FVector3i                  DstOffset = {0, 0, 0}; // 目标偏移量
    FVector3i                  Extent    = {0, 0, 0}; // 复制范围
};

struct FRHIBufferImageCopyRegion
{
    UInt64                     BufferOffset      = 0;   // 缓冲区偏移量（字节）
    UInt32                     BufferRowLength   = 0;   // 缓冲区行长度（像素）
    UInt32                     BufferImageHeight = 0;   // 缓冲区图像高度（像素）
    FRHIImageSubresourceLayers ImageSubresource;        // 图像子资源层
    FVector3i                  ImageOffset = {0, 0, 0}; // 图像偏移量
    FVector3i                  ImageExtent = {0, 0, 0}; // 图像范围
};

struct FRHIImageSubresourceRange
{
    ERHIImageAspect AspectMask     = ERHIImageAspect::Color; // 方面掩码
    UInt32          BaseMipLevel   = 0;                      // 基础MIP级别
    UInt32          LevelCount     = 1;                      // MIP级别数量
    UInt32          BaseArrayLayer = 0;                      // 基础数组层
    UInt32          LayerCount     = 1;                      // 层数量
};

struct FRHIMemoryBarrier
{
    ERHIAccessFlag SrcAccessMask = ERHIAccessFlag::None; // 源访问掩码
    ERHIAccessFlag DstAccessMask = ERHIAccessFlag::None; // 目标访问掩码
};

struct FRHIBufferMemoryBarrier
{
    ERHIAccessFlag SrcAccessMask = ERHIAccessFlag::None; // 源访问掩码
    ERHIAccessFlag DstAccessMask = ERHIAccessFlag::None; // 目标访问掩码
    FRHIBuffer     Buffer;                               // 缓冲区
    UInt64         Offset = 0;                           // 偏移量（字节）
    UInt64         Size   = 0;                           // 大小（字节），0表示到缓冲区末尾
};

struct FRHIImageMemoryBarrier
{
    ERHIAccessFlag            SrcAccessMask = ERHIAccessFlag::None;       // 源访问掩码
    ERHIAccessFlag            DstAccessMask = ERHIAccessFlag::None;       // 目标访问掩码
    ERHIImageLayout           OldLayout     = ERHIImageLayout::Undefined; // 旧布局
    ERHIImageLayout           NewLayout     = ERHIImageLayout::Undefined; // 新布局
    FRHIImage                 Image;                                      // 图像
    FRHIImageSubresourceRange SubresourceRange;                           // 子资源范围
};

struct FRHIRenderPassBeginInfo
{
    void*             RenderPass  = nullptr; // 渲染通道（暂时使用void*，后续改为FRHIRenderPass）
    void*             Framebuffer = nullptr; // 帧缓冲区（暂时使用void*，后续改为FRHIFramebuffer）
    FRHIRect2D        RenderArea;            // 渲染区域
    TArray<FVector4f> ClearValues;           // 清除值数组
};