#pragma once

#include "Core/Container/Array.h"
#include "Core/Utility/Macros.h"
#include "Math/Rect2D.h"
#include "Math/Vector.h"
#include "RHIBuffer.h"
#include "RHIDescriptorSet.h"
#include "RHIImage.h"
#include "RHIImageView.h"
#include "RHIPipeline.h"
#include <utility>

// 前向声明
struct FRHIViewport;
struct FRHIBufferCopyRegion;
struct FRHIImageCopyRegion;
struct FRHIBufferImageCopyRegion;
struct FRHIImageSubresourceRange;
struct FRHIMemoryBarrier;
struct FRHIBufferMemoryBarrier;
struct FRHIImageMemoryBarrier;
struct FRHIRenderPassBeginInfo;

// ERHIAccessFlag, ERHIPipelineStageFlag, ERHIDependencyFlag 定义在 RHICommandBuffer.h 中（避免循环依赖）
enum class ERHIAccessFlag : UInt32;
enum class ERHIPipelineStageFlag : UInt32;
enum class ERHIDependencyFlag : UInt32;

// 命令缓冲区使用标志（定义在这里以避免循环依赖）
enum class ERHICommandBufferUsageFlag : UInt32
{
    None               = 0,
    OneTimeSubmit      = 1 << 0, // 一次性提交（命令缓冲区只使用一次）
    RenderPassContinue = 1 << 1, // 在渲染通道中继续（仅用于辅助命令缓冲区）
    SimultaneousUse    = 1 << 2, // 同时使用（命令缓冲区可以多次提交）
};
HK_ENABLE_BITMASK_OPERATORS(ERHICommandBufferUsageFlag)

// 命令类型枚举
enum class ERHICommandType : UInt32
{
    None = 0,

    // 生命周期命令
    Begin,
    End,
    Reset,

    // 管线绑定命令
    BindPipeline,
    BindComputePipeline,

    // 描述符集绑定命令
    BindDescriptorSet,
    BindDescriptorSets,

    // 顶点和索引缓冲区绑定命令
    BindVertexBuffer,
    BindVertexBuffers,
    BindIndexBuffer,

    // 绘制命令
    Draw,
    DrawIndexed,
    DrawIndirect,
    DrawIndexedIndirect,

    // 计算调度命令
    Dispatch,
    DispatchIndirect,

    // 缓冲区复制命令
    CopyBuffer,
    CopyImage,
    CopyBufferToImage,
    CopyImageToBuffer,

    // 图像清除命令
    ClearColorImage,
    ClearDepthStencilImage,

    // 管线屏障和同步命令
    PipelineBarrier,

    // 视口和裁剪命令
    SetViewport,
    SetScissor,

    // 推送常量命令
    PushConstants,

    // 渲染通道命令（Dynamic Rendering）
    BeginRendering,
    EndRendering,

    Count,
};

// 命令基类
struct FRHICommand
{
    ERHICommandType CommandType = ERHICommandType::None;

    virtual ~FRHICommand() = default;
};

// ============================================================================
// 生命周期命令
// ============================================================================

struct FRHICommand_Begin : FRHICommand
{
    ERHICommandBufferUsageFlag UsageFlags = ERHICommandBufferUsageFlag::None;

    FRHICommand_Begin() : FRHICommand()
    {
        CommandType = ERHICommandType::Begin;
    }
};

struct FRHICommand_End : FRHICommand
{
    FRHICommand_End() : FRHICommand()
    {
        CommandType = ERHICommandType::End;
    }
};

struct FRHICommand_Reset : FRHICommand
{
    bool ReleaseResources = false;

    FRHICommand_Reset(const bool InReleaseResources = false) : FRHICommand(), ReleaseResources(InReleaseResources)
    {
        CommandType = ERHICommandType::Reset;
    }
};

// ============================================================================
// 管线绑定命令
// ============================================================================

struct FRHICommand_BindPipeline : FRHICommand
{
    FRHIPipeline Pipeline;

    FRHICommand_BindPipeline(FRHIPipeline InPipeline) : FRHICommand(), Pipeline(std::move(InPipeline))
    {
        CommandType = ERHICommandType::BindPipeline;
    }
};

struct FRHICommand_BindComputePipeline : FRHICommand
{
    FRHIPipeline Pipeline;

    FRHICommand_BindComputePipeline(FRHIPipeline InPipeline) : FRHICommand(), Pipeline(std::move(InPipeline))
    {
        CommandType = ERHICommandType::BindComputePipeline;
    }
};

// ============================================================================
// 描述符集绑定命令
// ============================================================================

struct FRHICommand_BindDescriptorSet : FRHICommand
{
    ERHIPipelineType   PipelineType;
    FRHIPipelineLayout Layout;
    FRHIDescriptorSet  DescriptorSet;
    UInt32             FirstSet;

    FRHICommand_BindDescriptorSet(const ERHIPipelineType InPipelineType, FRHIPipelineLayout InLayout,
                                  FRHIDescriptorSet InDescriptorSet, const UInt32 InFirstSet)
        : FRHICommand(), PipelineType(InPipelineType), Layout(std::move(InLayout)),
          DescriptorSet(std::move(InDescriptorSet)), FirstSet(InFirstSet)
    {
        CommandType = ERHICommandType::BindDescriptorSet;
    }
};

struct FRHICommand_BindDescriptorSets : FRHICommand
{
    ERHIPipelineType          PipelineType;
    FRHIPipelineLayout        Layout;
    TArray<FRHIDescriptorSet> DescriptorSets;
    UInt32                    FirstSet;

    FRHICommand_BindDescriptorSets(const ERHIPipelineType InPipelineType, FRHIPipelineLayout InLayout,
                                   const TArray<FRHIDescriptorSet>& InDescriptorSets, const UInt32 InFirstSet)
        : FRHICommand(), PipelineType(InPipelineType), Layout(std::move(InLayout)), DescriptorSets(InDescriptorSets),
          FirstSet(InFirstSet)
    {
        CommandType = ERHICommandType::BindDescriptorSets;
    }
};

// ============================================================================
// 顶点和索引缓冲区绑定命令
// ============================================================================

struct FRHICommand_BindVertexBuffer : FRHICommand
{
    UInt32     Binding;
    FRHIBuffer Buffer;
    UInt64     Offset;

    FRHICommand_BindVertexBuffer(const UInt32 InBinding, FRHIBuffer InBuffer, const UInt64 InOffset = 0)
        : FRHICommand(), Binding(InBinding), Buffer(std::move(InBuffer)), Offset(InOffset)
    {
        CommandType = ERHICommandType::BindVertexBuffer;
    }
};

struct FRHICommand_BindVertexBuffers : FRHICommand
{
    UInt32             FirstBinding;
    TArray<FRHIBuffer> Buffers;
    TArray<UInt64>     Offsets;

    FRHICommand_BindVertexBuffers(const UInt32 InFirstBinding, const TArray<FRHIBuffer>& InBuffers,
                                  const TArray<UInt64>& InOffsets)
        : FRHICommand(), FirstBinding(InFirstBinding), Buffers(InBuffers), Offsets(InOffsets)
    {
        CommandType = ERHICommandType::BindVertexBuffers;
    }
};

struct FRHICommand_BindIndexBuffer : FRHICommand
{
    FRHIBuffer Buffer;
    UInt64     Offset;
    bool       bIs32Bit;

    FRHICommand_BindIndexBuffer(FRHIBuffer InBuffer, const UInt64 InOffset = 0, const bool InIs32Bit = false)
        : FRHICommand(), Buffer(std::move(InBuffer)), Offset(InOffset), bIs32Bit(InIs32Bit)
    {
        CommandType = ERHICommandType::BindIndexBuffer;
    }
};

// ============================================================================
// 绘制命令
// ============================================================================

struct FRHICommand_Draw : FRHICommand
{
    UInt32 VertexCount;
    UInt32 InstanceCount;
    UInt32 FirstVertex;
    UInt32 FirstInstance;

    FRHICommand_Draw(const UInt32 InVertexCount, const UInt32 InInstanceCount = 1, const UInt32 InFirstVertex = 0,
                     const UInt32 InFirstInstance = 0)
        : FRHICommand(), VertexCount(InVertexCount), InstanceCount(InInstanceCount), FirstVertex(InFirstVertex),
          FirstInstance(InFirstInstance)
    {
        CommandType = ERHICommandType::Draw;
    }
};

struct FRHICommand_DrawIndexed : FRHICommand
{
    UInt32 IndexCount;
    UInt32 InstanceCount;
    UInt32 FirstIndex;
    Int32  VertexOffset;
    UInt32 FirstInstance;

    FRHICommand_DrawIndexed(const UInt32 InIndexCount, const UInt32 InInstanceCount = 1, const UInt32 InFirstIndex = 0,
                            const Int32 InVertexOffset = 0, const UInt32 InFirstInstance = 0)
        : FRHICommand(), IndexCount(InIndexCount), InstanceCount(InInstanceCount), FirstIndex(InFirstIndex),
          VertexOffset(InVertexOffset), FirstInstance(InFirstInstance)
    {
        CommandType = ERHICommandType::DrawIndexed;
    }
};

struct FRHICommand_DrawIndirect : FRHICommand
{
    FRHIBuffer Buffer;
    UInt64     Offset;
    UInt32     DrawCount;
    UInt32     Stride;

    FRHICommand_DrawIndirect(FRHIBuffer InBuffer, const UInt64 InOffset, const UInt32 InDrawCount = 1,
                             const UInt32 InStride = 0)
        : FRHICommand(), Buffer(std::move(InBuffer)), Offset(InOffset), DrawCount(InDrawCount), Stride(InStride)
    {
        CommandType = ERHICommandType::DrawIndirect;
    }
};

struct FRHICommand_DrawIndexedIndirect : FRHICommand
{
    FRHIBuffer Buffer;
    UInt64     Offset;
    UInt32     DrawCount;
    UInt32     Stride;

    FRHICommand_DrawIndexedIndirect(FRHIBuffer InBuffer, const UInt64 InOffset, const UInt32 InDrawCount = 1,
                                    const UInt32 InStride = 0)
        : FRHICommand(), Buffer(std::move(InBuffer)), Offset(InOffset), DrawCount(InDrawCount), Stride(InStride)
    {
        CommandType = ERHICommandType::DrawIndexedIndirect;
    }
};

// ============================================================================
// 计算调度命令
// ============================================================================

struct FRHICommand_Dispatch : FRHICommand
{
    UInt32 GroupCountX;
    UInt32 GroupCountY;
    UInt32 GroupCountZ;

    FRHICommand_Dispatch(const UInt32 InGroupCountX, const UInt32 InGroupCountY = 1, const UInt32 InGroupCountZ = 1)
        : FRHICommand(), GroupCountX(InGroupCountX), GroupCountY(InGroupCountY), GroupCountZ(InGroupCountZ)
    {
        CommandType = ERHICommandType::Dispatch;
    }
};

struct FRHICommand_DispatchIndirect : FRHICommand
{
    FRHIBuffer Buffer;
    UInt64     Offset;

    FRHICommand_DispatchIndirect(FRHIBuffer InBuffer, const UInt64 InOffset)
        : FRHICommand(), Buffer(std::move(InBuffer)), Offset(InOffset)
    {
        CommandType = ERHICommandType::DispatchIndirect;
    }
};

// ============================================================================
// 缓冲区复制命令
// ============================================================================

struct FRHICommand_CopyBuffer : FRHICommand
{
    FRHIBuffer                   SrcBuffer;
    FRHIBuffer                   DstBuffer;
    TArray<FRHIBufferCopyRegion> Regions;

    FRHICommand_CopyBuffer(FRHIBuffer InSrcBuffer, FRHIBuffer InDstBuffer,
                           const TArray<FRHIBufferCopyRegion>& InRegions)
        : FRHICommand(), SrcBuffer(std::move(InSrcBuffer)), DstBuffer(std::move(InDstBuffer)), Regions(InRegions)
    {
        CommandType = ERHICommandType::CopyBuffer;
    }
};

struct FRHICommand_CopyImage : FRHICommand
{
    FRHIImage                   SrcImage;
    FRHIImage                   DstImage;
    TArray<FRHIImageCopyRegion> Regions;

    FRHICommand_CopyImage(FRHIImage InSrcImage, FRHIImage InDstImage, const TArray<FRHIImageCopyRegion>& InRegions)
        : FRHICommand(), SrcImage(std::move(InSrcImage)), DstImage(std::move(InDstImage)), Regions(InRegions)
    {
        CommandType = ERHICommandType::CopyImage;
    }
};

struct FRHICommand_CopyBufferToImage : FRHICommand
{
    FRHIBuffer                        SrcBuffer;
    FRHIImage                         DstImage;
    TArray<FRHIBufferImageCopyRegion> Regions;

    FRHICommand_CopyBufferToImage(FRHIBuffer InSrcBuffer, FRHIImage InDstImage,
                                  const TArray<FRHIBufferImageCopyRegion>& InRegions)
        : FRHICommand(), SrcBuffer(std::move(InSrcBuffer)), DstImage(std::move(InDstImage)), Regions(InRegions)
    {
        CommandType = ERHICommandType::CopyBufferToImage;
    }
};

struct FRHICommand_CopyImageToBuffer : FRHICommand
{
    FRHIImage                         SrcImage;
    FRHIBuffer                        DstBuffer;
    TArray<FRHIBufferImageCopyRegion> Regions;

    FRHICommand_CopyImageToBuffer(FRHIImage InSrcImage, FRHIBuffer InDstBuffer,
                                  const TArray<FRHIBufferImageCopyRegion>& InRegions)
        : FRHICommand(), SrcImage(std::move(InSrcImage)), DstBuffer(std::move(InDstBuffer)), Regions(InRegions)
    {
        CommandType = ERHICommandType::CopyImageToBuffer;
    }
};

// ============================================================================
// 图像清除命令
// ============================================================================

struct FRHICommand_ClearColorImage : FRHICommand
{
    FRHIImage                         Image;
    FVector4f                         Color;
    TArray<FRHIImageSubresourceRange> Ranges;

    FRHICommand_ClearColorImage(FRHIImage InImage, const FVector4f& InColor,
                                const TArray<FRHIImageSubresourceRange>& InRanges)
        : FRHICommand(), Image(std::move(InImage)), Color(InColor), Ranges(InRanges)
    {
        CommandType = ERHICommandType::ClearColorImage;
    }
};

struct FRHICommand_ClearDepthStencilImage : FRHICommand
{
    FRHIImage                         Image;
    float                             Depth;
    UInt32                            Stencil;
    TArray<FRHIImageSubresourceRange> Ranges;

    FRHICommand_ClearDepthStencilImage(FRHIImage InImage, const float InDepth, const UInt32 InStencil,
                                       const TArray<FRHIImageSubresourceRange>& InRanges)
        : FRHICommand(), Image(std::move(InImage)), Depth(InDepth), Stencil(InStencil), Ranges(InRanges)
    {
        CommandType = ERHICommandType::ClearDepthStencilImage;
    }
};

// ============================================================================
// 管线屏障和同步命令
// ============================================================================

struct FRHICommand_PipelineBarrier : FRHICommand
{
    ERHIPipelineStageFlag           SrcStageMask;
    ERHIPipelineStageFlag           DstStageMask;
    ERHIDependencyFlag              DependencyFlags;
    TArray<FRHIMemoryBarrier>       MemoryBarriers;
    TArray<FRHIBufferMemoryBarrier> BufferMemoryBarriers;
    TArray<FRHIImageMemoryBarrier>  ImageMemoryBarriers;

    FRHICommand_PipelineBarrier(const ERHIPipelineStageFlag InSrcStageMask, const ERHIPipelineStageFlag InDstStageMask,
                                const ERHIDependencyFlag InDependencyFlags, const TArray<FRHIMemoryBarrier>& InMemoryBarriers,
                                const TArray<FRHIBufferMemoryBarrier>& InBufferMemoryBarriers,
                                const TArray<FRHIImageMemoryBarrier>&  InImageMemoryBarriers)
        : FRHICommand(), SrcStageMask(InSrcStageMask), DstStageMask(InDstStageMask), DependencyFlags(InDependencyFlags),
          MemoryBarriers(InMemoryBarriers), BufferMemoryBarriers(InBufferMemoryBarriers),
          ImageMemoryBarriers(InImageMemoryBarriers)
    {
        CommandType = ERHICommandType::PipelineBarrier;
    }
};

// ============================================================================
// 视口和裁剪命令
// ============================================================================

struct FRHICommand_SetViewport : FRHICommand
{
    UInt32               FirstViewport;
    TArray<FRHIViewport> Viewports;

    FRHICommand_SetViewport(const UInt32 InFirstViewport, const TArray<FRHIViewport>& InViewports)
        : FRHICommand(), FirstViewport(InFirstViewport), Viewports(InViewports)
    {
        CommandType = ERHICommandType::SetViewport;
    }
};

struct FRHICommand_SetScissor : FRHICommand
{
    UInt32             FirstScissor;
    TArray<FRHIRect2D> Scissors;

    FRHICommand_SetScissor(const UInt32 InFirstScissor, const TArray<FRHIRect2D>& InScissors)
        : FRHICommand(), FirstScissor(InFirstScissor), Scissors(InScissors)
    {
        CommandType = ERHICommandType::SetScissor;
    }
};

// ============================================================================
// 推送常量命令
// ============================================================================

struct FRHICommand_PushConstants : FRHICommand
{
    FRHIPipelineLayout Layout;
    UInt32             StageFlags;
    UInt32             Offset;
    UInt32             Size;
    TArray<UInt8>      Data; // 存储推送常量的数据

    FRHICommand_PushConstants(FRHIPipelineLayout InLayout, const UInt32 InStageFlags, const UInt32 InOffset,
                              const UInt32 InSize, const void* InData)
        : FRHICommand(), Layout(std::move(InLayout)), StageFlags(InStageFlags), Offset(InOffset), Size(InSize)
    {
        CommandType = ERHICommandType::PushConstants;
        Data.Resize(Size);
        if (InData)
        {
            memcpy(Data.Data(), InData, Size);
        }
    }
};

// ============================================================================
// Dynamic Rendering 命令
// ============================================================================

// 加载操作
enum class ERHIAttachmentLoadOp : UInt32
{
    Load     = 0, // 加载现有内容
    Clear    = 1, // 清除为指定的清除值
    DontCare = 2, // 不关心现有内容
};

// 存储操作
enum class ERHIAttachmentStoreOp : UInt32
{
    Store    = 0, // 存储渲染结果
    DontCare = 1, // 不关心渲染结果
};

// 渲染附件信息
struct FRHIRenderingAttachmentInfo
{
    FRHIImageView          ImageView;
    ERHIImageLayout        ImageLayout  = ERHIImageLayout::ColorAttachmentOptimal;
    ERHIAttachmentLoadOp   LoadOp       = ERHIAttachmentLoadOp::Clear;
    ERHIAttachmentStoreOp  StoreOp      = ERHIAttachmentStoreOp::Store;
    FVector4f              ClearValue   = FVector4f(0.0f, 0.0f, 0.0f, 1.0f);
};

struct FRHICommand_BeginRendering : FRHICommand
{
    // Dynamic Rendering 相关信息
    TArray<FRHIRenderingAttachmentInfo> ColorAttachments;     // 颜色附件数组
    FRHIRenderingAttachmentInfo         DepthAttachment;      // 深度附件（可选）
    FRHIRenderingAttachmentInfo         StencilAttachment;    // 模板附件（可选）
    bool                                bHasDepthAttachment   = false;
    bool                                bHasStencilAttachment = false;
    FRHIRect2D                          RenderArea;           // 渲染区域
    UInt32                              LayerCount            = 1;

    FRHICommand_BeginRendering() : FRHICommand()
    {
        CommandType = ERHICommandType::BeginRendering;
    }
};

struct FRHICommand_EndRendering : FRHICommand
{
    FRHICommand_EndRendering() : FRHICommand()
    {
        CommandType = ERHICommandType::EndRendering;
    }
};
