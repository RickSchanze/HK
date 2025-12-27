#pragma once

#include "Core/Container/Array.h"
#include "Math/Vector.h"
#include "Math/Rect2D.h"
#include "RHIBuffer.h"
#include "RHIImage.h"
#include "RHIPipeline.h"
#include "RHIDescriptorSet.h"

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

    FRHICommand_Reset(bool InReleaseResources = false) : FRHICommand(), ReleaseResources(InReleaseResources)
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

    FRHICommand_BindPipeline(const FRHIPipeline& InPipeline) : FRHICommand(), Pipeline(InPipeline)
    {
        CommandType = ERHICommandType::BindPipeline;
    }
};

struct FRHICommand_BindComputePipeline : FRHICommand
{
    FRHIPipeline Pipeline;

    FRHICommand_BindComputePipeline(const FRHIPipeline& InPipeline) : FRHICommand(), Pipeline(InPipeline)
    {
        CommandType = ERHICommandType::BindComputePipeline;
    }
};

// ============================================================================
// 描述符集绑定命令
// ============================================================================

struct FRHICommand_BindDescriptorSet : FRHICommand
{
    ERHIPipelineType      PipelineType;
    FRHIPipelineLayout    Layout;
    FRHIDescriptorSet     DescriptorSet;
    UInt32                FirstSet;

    FRHICommand_BindDescriptorSet(ERHIPipelineType InPipelineType, const FRHIPipelineLayout& InLayout,
                                  const FRHIDescriptorSet& InDescriptorSet, UInt32 InFirstSet)
        : FRHICommand(), PipelineType(InPipelineType), Layout(InLayout), DescriptorSet(InDescriptorSet),
          FirstSet(InFirstSet)
    {
        CommandType = ERHICommandType::BindDescriptorSet;
    }
};

struct FRHICommand_BindDescriptorSets : FRHICommand
{
    ERHIPipelineType           PipelineType;
    FRHIPipelineLayout         Layout;
    TArray<FRHIDescriptorSet>  DescriptorSets;
    UInt32                     FirstSet;

    FRHICommand_BindDescriptorSets(ERHIPipelineType InPipelineType, const FRHIPipelineLayout& InLayout,
                                   const TArray<FRHIDescriptorSet>& InDescriptorSets, UInt32 InFirstSet)
        : FRHICommand(), PipelineType(InPipelineType), Layout(InLayout), DescriptorSets(InDescriptorSets),
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

    FRHICommand_BindVertexBuffer(UInt32 InBinding, const FRHIBuffer& InBuffer, UInt64 InOffset = 0)
        : FRHICommand(), Binding(InBinding), Buffer(InBuffer), Offset(InOffset)
    {
        CommandType = ERHICommandType::BindVertexBuffer;
    }
};

struct FRHICommand_BindVertexBuffers : FRHICommand
{
    UInt32                FirstBinding;
    TArray<FRHIBuffer>    Buffers;
    TArray<UInt64>       Offsets;

    FRHICommand_BindVertexBuffers(UInt32 InFirstBinding, const TArray<FRHIBuffer>& InBuffers,
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

    FRHICommand_BindIndexBuffer(const FRHIBuffer& InBuffer, UInt64 InOffset = 0, bool InIs32Bit = false)
        : FRHICommand(), Buffer(InBuffer), Offset(InOffset), bIs32Bit(InIs32Bit)
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

    FRHICommand_Draw(UInt32 InVertexCount, UInt32 InInstanceCount = 1, UInt32 InFirstVertex = 0,
                     UInt32 InFirstInstance = 0)
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

    FRHICommand_DrawIndexed(UInt32 InIndexCount, UInt32 InInstanceCount = 1, UInt32 InFirstIndex = 0,
                             Int32 InVertexOffset = 0, UInt32 InFirstInstance = 0)
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

    FRHICommand_DrawIndirect(const FRHIBuffer& InBuffer, UInt64 InOffset, UInt32 InDrawCount = 1,
                              UInt32 InStride = 0)
        : FRHICommand(), Buffer(InBuffer), Offset(InOffset), DrawCount(InDrawCount), Stride(InStride)
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

    FRHICommand_DrawIndexedIndirect(const FRHIBuffer& InBuffer, UInt64 InOffset, UInt32 InDrawCount = 1,
                                     UInt32 InStride = 0)
        : FRHICommand(), Buffer(InBuffer), Offset(InOffset), DrawCount(InDrawCount), Stride(InStride)
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

    FRHICommand_Dispatch(UInt32 InGroupCountX, UInt32 InGroupCountY = 1, UInt32 InGroupCountZ = 1)
        : FRHICommand(), GroupCountX(InGroupCountX), GroupCountY(InGroupCountY), GroupCountZ(InGroupCountZ)
    {
        CommandType = ERHICommandType::Dispatch;
    }
};

struct FRHICommand_DispatchIndirect : FRHICommand
{
    FRHIBuffer Buffer;
    UInt64     Offset;

    FRHICommand_DispatchIndirect(const FRHIBuffer& InBuffer, UInt64 InOffset)
        : FRHICommand(), Buffer(InBuffer), Offset(InOffset)
    {
        CommandType = ERHICommandType::DispatchIndirect;
    }
};

// ============================================================================
// 缓冲区复制命令
// ============================================================================

struct FRHICommand_CopyBuffer : FRHICommand
{
    FRHIBuffer                      SrcBuffer;
    FRHIBuffer                      DstBuffer;
    TArray<FRHIBufferCopyRegion>    Regions;

    FRHICommand_CopyBuffer(const FRHIBuffer& InSrcBuffer, const FRHIBuffer& InDstBuffer,
                           const TArray<FRHIBufferCopyRegion>& InRegions)
        : FRHICommand(), SrcBuffer(InSrcBuffer), DstBuffer(InDstBuffer), Regions(InRegions)
    {
        CommandType = ERHICommandType::CopyBuffer;
    }
};

struct FRHICommand_CopyImage : FRHICommand
{
    FRHIImage                    SrcImage;
    FRHIImage                    DstImage;
    TArray<FRHIImageCopyRegion>  Regions;

    FRHICommand_CopyImage(const FRHIImage& InSrcImage, const FRHIImage& InDstImage,
                          const TArray<FRHIImageCopyRegion>& InRegions)
        : FRHICommand(), SrcImage(InSrcImage), DstImage(InDstImage), Regions(InRegions)
    {
        CommandType = ERHICommandType::CopyImage;
    }
};

struct FRHICommand_CopyBufferToImage : FRHICommand
{
    FRHIBuffer                          SrcBuffer;
    FRHIImage                          DstImage;
    TArray<FRHIBufferImageCopyRegion>   Regions;

    FRHICommand_CopyBufferToImage(const FRHIBuffer& InSrcBuffer, const FRHIImage& InDstImage,
                                  const TArray<FRHIBufferImageCopyRegion>& InRegions)
        : FRHICommand(), SrcBuffer(InSrcBuffer), DstImage(InDstImage), Regions(InRegions)
    {
        CommandType = ERHICommandType::CopyBufferToImage;
    }
};

struct FRHICommand_CopyImageToBuffer : FRHICommand
{
    FRHIImage                          SrcImage;
    FRHIBuffer                         DstBuffer;
    TArray<FRHIBufferImageCopyRegion>  Regions;

    FRHICommand_CopyImageToBuffer(const FRHIImage& InSrcImage, const FRHIBuffer& InDstBuffer,
                                  const TArray<FRHIBufferImageCopyRegion>& InRegions)
        : FRHICommand(), SrcImage(InSrcImage), DstBuffer(InDstBuffer), Regions(InRegions)
    {
        CommandType = ERHICommandType::CopyImageToBuffer;
    }
};

// ============================================================================
// 图像清除命令
// ============================================================================

struct FRHICommand_ClearColorImage : FRHICommand
{
    FRHIImage                          Image;
    FVector4f                          Color;
    TArray<FRHIImageSubresourceRange>  Ranges;

    FRHICommand_ClearColorImage(const FRHIImage& InImage, const FVector4f& InColor,
                                const TArray<FRHIImageSubresourceRange>& InRanges)
        : FRHICommand(), Image(InImage), Color(InColor), Ranges(InRanges)
    {
        CommandType = ERHICommandType::ClearColorImage;
    }
};

struct FRHICommand_ClearDepthStencilImage : FRHICommand
{
    FRHIImage                          Image;
    float                              Depth;
    UInt32                             Stencil;
    TArray<FRHIImageSubresourceRange>  Ranges;

    FRHICommand_ClearDepthStencilImage(const FRHIImage& InImage, float InDepth, UInt32 InStencil,
                                        const TArray<FRHIImageSubresourceRange>& InRanges)
        : FRHICommand(), Image(InImage), Depth(InDepth), Stencil(InStencil), Ranges(InRanges)
    {
        CommandType = ERHICommandType::ClearDepthStencilImage;
    }
};

// ============================================================================
// 管线屏障和同步命令
// ============================================================================

struct FRHICommand_PipelineBarrier : FRHICommand
{
    UInt32                              SrcStageMask;
    UInt32                              DstStageMask;
    UInt32                              DependencyFlags;
    TArray<FRHIMemoryBarrier>           MemoryBarriers;
    TArray<FRHIBufferMemoryBarrier>     BufferMemoryBarriers;
    TArray<FRHIImageMemoryBarrier>      ImageMemoryBarriers;

    FRHICommand_PipelineBarrier(UInt32 InSrcStageMask, UInt32 InDstStageMask, UInt32 InDependencyFlags,
                                 const TArray<FRHIMemoryBarrier>& InMemoryBarriers,
                                 const TArray<FRHIBufferMemoryBarrier>& InBufferMemoryBarriers,
                                 const TArray<FRHIImageMemoryBarrier>& InImageMemoryBarriers)
        : FRHICommand(), SrcStageMask(InSrcStageMask), DstStageMask(InDstStageMask),
          DependencyFlags(InDependencyFlags), MemoryBarriers(InMemoryBarriers),
          BufferMemoryBarriers(InBufferMemoryBarriers), ImageMemoryBarriers(InImageMemoryBarriers)
    {
        CommandType = ERHICommandType::PipelineBarrier;
    }
};

// ============================================================================
// 视口和裁剪命令
// ============================================================================

struct FRHICommand_SetViewport : FRHICommand
{
    UInt32                  FirstViewport;
    TArray<FRHIViewport>    Viewports;

    FRHICommand_SetViewport(UInt32 InFirstViewport, const TArray<FRHIViewport>& InViewports)
        : FRHICommand(), FirstViewport(InFirstViewport), Viewports(InViewports)
    {
        CommandType = ERHICommandType::SetViewport;
    }
};

struct FRHICommand_SetScissor : FRHICommand
{
    UInt32              FirstScissor;
    TArray<FRHIRect2D>  Scissors;

    FRHICommand_SetScissor(UInt32 InFirstScissor, const TArray<FRHIRect2D>& InScissors)
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

    FRHICommand_PushConstants(const FRHIPipelineLayout& InLayout, UInt32 InStageFlags, UInt32 InOffset,
                              UInt32 InSize, const void* InData)
        : FRHICommand(), Layout(InLayout), StageFlags(InStageFlags), Offset(InOffset), Size(InSize)
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

struct FRHICommand_BeginRendering : FRHICommand
{
    // Dynamic Rendering 相关信息
    TArray<FRHIImageView>    ColorAttachments;      // 颜色附件
    FRHIImageView            DepthAttachment;        // 深度附件（可选）
    FRHIImageView            StencilAttachment;      // 模板附件（可选）
    FRHIRect2D               RenderArea;             // 渲染区域
    ERHIImageLayout          DepthAttachmentLayout = ERHIImageLayout::Undefined;
    ERHIImageLayout          StencilAttachmentLayout = ERHIImageLayout::Undefined;
    bool                     bResolveAttachments = false; // 是否使用解析附件

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

