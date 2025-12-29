//
// Created by Admin on 2025/12/27.
//

#include "RHICommandBuffer.h"
#include "Core/Utility/UniquePtr.h"
#include "GfxDevice.h"

// 所有命令接口的实现都通过 AddOrExecuteCommand 来统一处理
// 实际的执行逻辑在 Vulkan 实现中

void FRHICommandBuffer::Begin(ERHICommandBufferUsageFlag UsageFlags)
{
    auto Cmd        = MakeUnique<FRHICommand_Begin>();
    Cmd->UsageFlags = UsageFlags;
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::End()
{
    auto Cmd = MakeUnique<FRHICommand_End>();
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::Reset(bool ReleaseResources)
{
    auto Cmd = MakeUnique<FRHICommand_Reset>(ReleaseResources);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::BindPipeline(const FRHIPipeline& Pipeline)
{
    auto Cmd = MakeUnique<FRHICommand_BindPipeline>(Pipeline);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::BindComputePipeline(const FRHIPipeline& Pipeline)
{
    auto Cmd = MakeUnique<FRHICommand_BindComputePipeline>(Pipeline);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::BindDescriptorSet(ERHIPipelineType PipelineType, const FRHIPipelineLayout& Layout,
                                          const FRHIDescriptorSet& DescriptorSet, UInt32 FirstSet)
{
    auto Cmd = MakeUnique<FRHICommand_BindDescriptorSet>(PipelineType, Layout, DescriptorSet, FirstSet);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::BindDescriptorSets(ERHIPipelineType PipelineType, const FRHIPipelineLayout& Layout,
                                           const TArray<FRHIDescriptorSet>& DescriptorSets, UInt32 FirstSet)
{
    auto Cmd = MakeUnique<FRHICommand_BindDescriptorSets>(PipelineType, Layout, DescriptorSets, FirstSet);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::BindVertexBuffer(UInt32 Binding, const FRHIBuffer& Buffer, UInt64 Offset)
{
    auto Cmd = MakeUnique<FRHICommand_BindVertexBuffer>(Binding, Buffer, Offset);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::BindVertexBuffers(UInt32 FirstBinding, const TArray<FRHIBuffer>& Buffers,
                                          const TArray<UInt64>& Offsets)
{
    auto Cmd = MakeUnique<FRHICommand_BindVertexBuffers>(FirstBinding, Buffers, Offsets);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::BindIndexBuffer(const FRHIBuffer& Buffer, UInt64 Offset, bool bIs32Bit)
{
    auto Cmd = MakeUnique<FRHICommand_BindIndexBuffer>(Buffer, Offset, bIs32Bit);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::Draw(UInt32 VertexCount, UInt32 InstanceCount, UInt32 FirstVertex, UInt32 FirstInstance)
{
    auto Cmd = MakeUnique<FRHICommand_Draw>(VertexCount, InstanceCount, FirstVertex, FirstInstance);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::DrawIndexed(UInt32 IndexCount, UInt32 InstanceCount, UInt32 FirstIndex, Int32 VertexOffset,
                                    UInt32 FirstInstance)
{
    auto Cmd = MakeUnique<FRHICommand_DrawIndexed>(IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::DrawIndirect(const FRHIBuffer& Buffer, UInt64 Offset, UInt32 DrawCount, UInt32 Stride)
{
    auto Cmd = MakeUnique<FRHICommand_DrawIndirect>(Buffer, Offset, DrawCount, Stride);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::DrawIndexedIndirect(const FRHIBuffer& Buffer, UInt64 Offset, UInt32 DrawCount, UInt32 Stride)
{
    auto Cmd = MakeUnique<FRHICommand_DrawIndexedIndirect>(Buffer, Offset, DrawCount, Stride);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::Dispatch(UInt32 GroupCountX, UInt32 GroupCountY, UInt32 GroupCountZ)
{
    auto Cmd = MakeUnique<FRHICommand_Dispatch>(GroupCountX, GroupCountY, GroupCountZ);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::DispatchIndirect(const FRHIBuffer& Buffer, UInt64 Offset)
{
    auto Cmd = MakeUnique<FRHICommand_DispatchIndirect>(Buffer, Offset);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::CopyBuffer(const FRHIBuffer& SrcBuffer, const FRHIBuffer& DstBuffer,
                                   const TArray<FRHIBufferCopyRegion>& Regions)
{
    auto Cmd = MakeUnique<FRHICommand_CopyBuffer>(SrcBuffer, DstBuffer, Regions);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::CopyImage(const FRHIImage& SrcImage, const FRHIImage& DstImage,
                                  const TArray<FRHIImageCopyRegion>& Regions)
{
    auto Cmd = MakeUnique<FRHICommand_CopyImage>(SrcImage, DstImage, Regions);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::CopyBufferToImage(const FRHIBuffer& SrcBuffer, const FRHIImage& DstImage,
                                          const TArray<FRHIBufferImageCopyRegion>& Regions)
{
    auto Cmd = MakeUnique<FRHICommand_CopyBufferToImage>(SrcBuffer, DstImage, Regions);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::CopyImageToBuffer(const FRHIImage& SrcImage, const FRHIBuffer& DstBuffer,
                                          const TArray<FRHIBufferImageCopyRegion>& Regions)
{
    auto Cmd = MakeUnique<FRHICommand_CopyImageToBuffer>(SrcImage, DstBuffer, Regions);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::ClearColorImage(const FRHIImage& Image, const FVector4f& Color,
                                        const TArray<FRHIImageSubresourceRange>& Ranges)
{
    auto Cmd = MakeUnique<FRHICommand_ClearColorImage>(Image, Color, Ranges);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::ClearDepthStencilImage(const FRHIImage& Image, float Depth, UInt32 Stencil,
                                               const TArray<FRHIImageSubresourceRange>& Ranges)
{
    auto Cmd = MakeUnique<FRHICommand_ClearDepthStencilImage>(Image, Depth, Stencil, Ranges);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::PipelineBarrier(ERHIPipelineStageFlag SrcStageMask, ERHIPipelineStageFlag DstStageMask,
                                        ERHIDependencyFlag                     DependencyFlags,
                                        const TArray<FRHIMemoryBarrier>&       MemoryBarriers,
                                        const TArray<FRHIBufferMemoryBarrier>& BufferMemoryBarriers,
                                        const TArray<FRHIImageMemoryBarrier>&  ImageMemoryBarriers)
{
    auto Cmd = MakeUnique<FRHICommand_PipelineBarrier>(SrcStageMask, DstStageMask, DependencyFlags, MemoryBarriers,
                                                       BufferMemoryBarriers, ImageMemoryBarriers);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::SetViewport(UInt32 FirstViewport, const TArray<FRHIViewport>& Viewports)
{
    auto Cmd = MakeUnique<FRHICommand_SetViewport>(FirstViewport, Viewports);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::SetScissor(UInt32 FirstScissor, const TArray<FRHIRect2D>& Scissors)
{
    auto Cmd = MakeUnique<FRHICommand_SetScissor>(FirstScissor, Scissors);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::PushConstants(const FRHIPipelineLayout& Layout, UInt32 StageFlags, UInt32 Offset, UInt32 Size,
                                      const void* Data)
{
    auto Cmd = MakeUnique<FRHICommand_PushConstants>(Layout, StageFlags, Offset, Size, Data);
    AddOrExecuteCommand(std::move(Cmd));
}

void FRHICommandBuffer::BeginRenderPass(const FRHIRenderPassBeginInfo& RenderPassBeginInfo,
                                        ERHICommandBufferLevel         Contents)
{
    // 注意：这里暂时保留 BeginRenderPass，但实际应该使用 BeginRendering（Dynamic Rendering）
    // TODO: 后续移除 BeginRenderPass，改用 BeginRendering
    HK_ASSERT_MSG(false, "BeginRenderPass is deprecated, use BeginRendering instead");
}

void FRHICommandBuffer::EndRenderPass()
{
    // 注意：这里暂时保留 EndRenderPass，但实际应该使用 EndRendering（Dynamic Rendering）
    // TODO: 后续移除 EndRenderPass，改用 EndRendering
    HK_ASSERT_MSG(false, "EndRenderPass is deprecated, use EndRendering instead");
}

// 执行所有排队的命令
void FRHICommandBuffer::Execute()
{
    if (ExecuteMode == ERHICommandExecuteMode::Immediate)
    {
        // Immediate 模式：什么也不做（命令已经立即执行）
        return;
    }
    if (ExecuteMode == ERHICommandExecuteMode::Deferred)
    {
        // Deferred 模式：在本线程执行所有排队的命令
        for (const auto& Cmd : CommandQueue)
        {
            if (Cmd)
            {
                ExecuteCommand(*Cmd);
            }
        }
        // 清空命令队列
        ClearCommands();
    }
    // Threaded 模式暂时忽略
}

// 清空命令队列
void FRHICommandBuffer::ClearCommands()
{
    CommandQueue.Clear();
}

// 添加命令到队列（Deferred 模式）或立即执行（Immediate 模式）
void FRHICommandBuffer::AddOrExecuteCommand(TUniquePtr<FRHICommand> Command)
{
    if (!Command)
    {
        return;
    }

    if (ExecuteMode == ERHICommandExecuteMode::Immediate)
    {
        // 立即执行命令
        ExecuteCommand(*Command);
    }
    else if (ExecuteMode == ERHICommandExecuteMode::Deferred)
    {
        // 添加到命令队列
        CommandQueue.Add(std::move(Command));
    }
    // Threaded 模式暂时忽略
}

// 执行命令（内部方法，由 GfxDevice 实现调用）
void FRHICommandBuffer::ExecuteCommand(const FRHICommand& Command)
{
    // 通过友元函数调用 GfxDevice 的实现
    if (FGfxDevice* Device = GetGfxDevice())
    {
        // 调用平台特定的实现（通过友元函数）
        // 实际的实现在 GfxDeviceVk::ExecuteCommand 中
        Device->ExecuteCommand(*this, Command);
    }
}
