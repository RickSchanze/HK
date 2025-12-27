//
// Created by Admin on 2025/12/27.
//

#include "Core/Logging/Logger.h"
#include "Core/Utility/Macros.h"
#include "GfxDeviceVk.h"
#include "RHI/RHICommand.h"
#include "RHI/RHICommandBuffer.h"
#include "RHI/RHICommandPool.h"
#include "RHI/RHIImageView.h"
#include <vector>

// ============================================================================
// CommandBuffer 创建和销毁
// ============================================================================

FRHICommandBuffer FGfxDeviceVk::CreateCommandBuffer(const FRHICommandPool&       Pool,
                                                    const FRHICommandBufferDesc& CommandBufferCreateInfo)
{
    FRHICommandBuffer CmdBuffer;

    if (!Pool.IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid command pool provided for command buffer creation");
        return CmdBuffer;
    }

    VkCommandPool PoolHandle = Pool.Handle.Cast<VkCommandPool>();
    vk::CommandPool VkPool = vk::CommandPool(PoolHandle);

    // 转换级别
    vk::CommandBufferLevel VkLevel = (CommandBufferCreateInfo.Level == ERHICommandBufferLevel::Primary)
                                         ? vk::CommandBufferLevel::ePrimary
                                         : vk::CommandBufferLevel::eSecondary;

    // 分配命令缓冲区
    vk::CommandBufferAllocateInfo  AllocInfo(VkPool, VkLevel, 1);
    std::vector<vk::CommandBuffer> VkCmdBuffers = Device.allocateCommandBuffers(AllocInfo);

    if (VkCmdBuffers.empty() || !VkCmdBuffers[0])
    {
        HK_LOG_ERROR(ELogcat::RHI, "Failed to allocate Vulkan command buffer");
        return CmdBuffer;
    }

    vk::CommandBuffer VkCmdBuffer = VkCmdBuffers[0];

    // 创建 RHI Handle（转换为 C 类型存储）
    CmdBuffer.Handle = FRHIHandleManager::GetRef().CreateRHIHandle(
        CommandBufferCreateInfo.DebugName, reinterpret_cast<void*>(static_cast<VkCommandBuffer>(VkCmdBuffer)));
    CmdBuffer.Level  = CommandBufferCreateInfo.Level;

    // 设置调试名称
    if (bDebugUtilsExtensionAvailable && !CommandBufferCreateInfo.DebugName.IsEmpty())
    {
        SetDebugName(VkCmdBuffer, vk::ObjectType::eCommandBuffer, CommandBufferCreateInfo.DebugName);
    }

    return CmdBuffer;
}

void FGfxDeviceVk::DestroyCommandBuffer(const FRHICommandPool& Pool, FRHICommandBuffer& CommandBuffer)
{
    if (!CommandBuffer.IsValid() || !Pool.IsValid())
    {
        return;
    }

    VkCommandPool PoolHandle = Pool.Handle.Cast<VkCommandPool>();
    vk::CommandPool   VkPool      = vk::CommandPool(PoolHandle);
    VkCommandBuffer CmdBufferHandle = CommandBuffer.Handle.Cast<VkCommandBuffer>();
    vk::CommandBuffer VkCmdBuffer = vk::CommandBuffer(CmdBufferHandle);

    // 释放命令缓冲区
    Device.freeCommandBuffers(VkPool, {VkCmdBuffer});

    FRHIHandleManager::GetRef().DestroyRHIHandle(CommandBuffer.Handle);
    CommandBuffer.Handle = FRHIHandle();
}

// ============================================================================
// 命令执行（核心逻辑）
// ============================================================================

void FGfxDeviceVk::ExecuteCommand(FRHICommandBuffer& CommandBuffer, const FRHICommand& Command)
{
    if (!CommandBuffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid command buffer");
        return;
    }

    VkCommandBuffer CmdBufferHandle = CommandBuffer.Handle.Cast<VkCommandBuffer>();
    vk::CommandBuffer VkCmdBuffer = vk::CommandBuffer(CmdBufferHandle);
    if (!VkCmdBuffer)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid Vulkan command buffer");
        return;
    }

    // 根据命令类型执行相应的 Vulkan 命令
    switch (Command.CommandType)
    {
        case ERHICommandType::Begin:
        {
            const auto&                Cmd = static_cast<const FRHICommand_Begin&>(Command);
            vk::CommandBufferBeginInfo BeginInfo;
            BeginInfo.flags = ConvertCommandBufferUsageFlags(Cmd.UsageFlags);
            VkCmdBuffer.begin(BeginInfo);
            CommandBuffer.bIsRecording = true;
            break;
        }

        case ERHICommandType::End:
        {
            VkCmdBuffer.end();
            CommandBuffer.bIsRecording = false;
            break;
        }

        case ERHICommandType::Reset:
        {
            const auto&                 Cmd   = static_cast<const FRHICommand_Reset&>(Command);
            vk::CommandBufferResetFlags Flags = Cmd.ReleaseResources ? vk::CommandBufferResetFlagBits::eReleaseResources
                                                                     : vk::CommandBufferResetFlags();
            VkCmdBuffer.reset(Flags);
            CommandBuffer.bIsRecording = false;
            break;
        }

        case ERHICommandType::BindPipeline:
        {
            const auto&  Cmd        = static_cast<const FRHICommand_BindPipeline&>(Command);
            VkPipeline Pipeline = Cmd.Pipeline.GetHandle().Cast<VkPipeline>();
            vk::Pipeline VkPipeline = vk::Pipeline(Pipeline);
            VkCmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, VkPipeline);
            break;
        }

        case ERHICommandType::BindComputePipeline:
        {
            const auto&  Cmd        = static_cast<const FRHICommand_BindComputePipeline&>(Command);
            VkPipeline Pipeline = Cmd.Pipeline.GetHandle().Cast<VkPipeline>();
            vk::Pipeline VkPipeline = vk::Pipeline(Pipeline);
            VkCmdBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, VkPipeline);
            break;
        }

        case ERHICommandType::BindDescriptorSet:
        {
            const auto&           Cmd       = static_cast<const FRHICommand_BindDescriptorSet&>(Command);
            VkPipelineLayout Layout = Cmd.Layout.GetHandle().Cast<VkPipelineLayout>();
            vk::PipelineLayout    VkLayout  = vk::PipelineLayout(Layout);
            VkDescriptorSet Set = Cmd.DescriptorSet.GetHandle().Cast<VkDescriptorSet>();
            vk::DescriptorSet     VkSet     = vk::DescriptorSet(Set);
            vk::PipelineBindPoint BindPoint = (Cmd.PipelineType == ERHIPipelineType::Graphics)
                                                  ? vk::PipelineBindPoint::eGraphics
                                                  : vk::PipelineBindPoint::eCompute;
            VkCmdBuffer.bindDescriptorSets(BindPoint, VkLayout, Cmd.FirstSet, {VkSet}, {});
            break;
        }

        case ERHICommandType::BindDescriptorSets:
        {
            const auto&               Cmd      = static_cast<const FRHICommand_BindDescriptorSets&>(Command);
            VkPipelineLayout Layout = Cmd.Layout.GetHandle().Cast<VkPipelineLayout>();
            vk::PipelineLayout        VkLayout = vk::PipelineLayout(Layout);
            TArray<vk::DescriptorSet> VkSets;
            VkSets.Reserve(Cmd.DescriptorSets.Size());
            for (const auto& Set : Cmd.DescriptorSets)
            {
                VkDescriptorSet DescriptorSet = Set.GetHandle().Cast<VkDescriptorSet>();
                VkSets.Add(vk::DescriptorSet(DescriptorSet));
            }
            vk::PipelineBindPoint BindPoint = (Cmd.PipelineType == ERHIPipelineType::Graphics)
                                                  ? vk::PipelineBindPoint::eGraphics
                                                  : vk::PipelineBindPoint::eCompute;
            VkCmdBuffer.bindDescriptorSets(BindPoint, VkLayout, Cmd.FirstSet,
                                           vk::ArrayProxy<const vk::DescriptorSet>(VkSets.Size(), VkSets.Data()), {});
            break;
        }

        case ERHICommandType::BindVertexBuffer:
        {
            const auto& Cmd      = static_cast<const FRHICommand_BindVertexBuffer&>(Command);
            VkBuffer Buffer = Cmd.Buffer.GetHandle().Cast<VkBuffer>();
            vk::Buffer  VkBuffer = vk::Buffer(Buffer);
            VkCmdBuffer.bindVertexBuffers(Cmd.Binding, {VkBuffer}, {Cmd.Offset});
            break;
        }

        case ERHICommandType::BindVertexBuffers:
        {
            const auto&        Cmd = static_cast<const FRHICommand_BindVertexBuffers&>(Command);
            TArray<vk::Buffer> VkBuffers;
            VkBuffers.Reserve(Cmd.Buffers.Size());
            for (const auto& Buffer : Cmd.Buffers)
            {
                VkBuffer Buf = Buffer.GetHandle().Cast<VkBuffer>();
                VkBuffers.Add(vk::Buffer(Buf));
            }
            VkCmdBuffer.bindVertexBuffers(Cmd.FirstBinding,
                                          vk::ArrayProxy<const vk::Buffer>(VkBuffers.Size(), VkBuffers.Data()),
                                          vk::ArrayProxy<const UInt64>(Cmd.Offsets.Size(), Cmd.Offsets.Data()));
            break;
        }

        case ERHICommandType::BindIndexBuffer:
        {
            const auto&   Cmd       = static_cast<const FRHICommand_BindIndexBuffer&>(Command);
            VkBuffer Buffer = Cmd.Buffer.GetHandle().Cast<VkBuffer>();
            vk::Buffer    VkBuffer  = vk::Buffer(Buffer);
            vk::IndexType IndexType = Cmd.bIs32Bit ? vk::IndexType::eUint32 : vk::IndexType::eUint16;
            VkCmdBuffer.bindIndexBuffer(VkBuffer, Cmd.Offset, IndexType);
            break;
        }

        case ERHICommandType::Draw:
        {
            const auto& Cmd = static_cast<const FRHICommand_Draw&>(Command);
            VkCmdBuffer.draw(Cmd.VertexCount, Cmd.InstanceCount, Cmd.FirstVertex, Cmd.FirstInstance);
            break;
        }

        case ERHICommandType::DrawIndexed:
        {
            const auto& Cmd = static_cast<const FRHICommand_DrawIndexed&>(Command);
            VkCmdBuffer.drawIndexed(Cmd.IndexCount, Cmd.InstanceCount, Cmd.FirstIndex, Cmd.VertexOffset,
                                    Cmd.FirstInstance);
            break;
        }

        case ERHICommandType::DrawIndirect:
        {
            const auto& Cmd      = static_cast<const FRHICommand_DrawIndirect&>(Command);
            VkBuffer Buffer = Cmd.Buffer.GetHandle().Cast<VkBuffer>();
            vk::Buffer  VkBuffer = vk::Buffer(Buffer);
            VkCmdBuffer.drawIndirect(VkBuffer, Cmd.Offset, Cmd.DrawCount, Cmd.Stride);
            break;
        }

        case ERHICommandType::DrawIndexedIndirect:
        {
            const auto& Cmd      = static_cast<const FRHICommand_DrawIndexedIndirect&>(Command);
            VkBuffer Buffer = Cmd.Buffer.GetHandle().Cast<VkBuffer>();
            vk::Buffer  VkBuffer = vk::Buffer(Buffer);
            VkCmdBuffer.drawIndexedIndirect(VkBuffer, Cmd.Offset, Cmd.DrawCount, Cmd.Stride);
            break;
        }

        case ERHICommandType::Dispatch:
        {
            const auto& Cmd = static_cast<const FRHICommand_Dispatch&>(Command);
            VkCmdBuffer.dispatch(Cmd.GroupCountX, Cmd.GroupCountY, Cmd.GroupCountZ);
            break;
        }

        case ERHICommandType::DispatchIndirect:
        {
            const auto& Cmd      = static_cast<const FRHICommand_DispatchIndirect&>(Command);
            VkBuffer Buffer = Cmd.Buffer.GetHandle().Cast<VkBuffer>();
            vk::Buffer  VkBuffer = vk::Buffer(Buffer);
            VkCmdBuffer.dispatchIndirect(VkBuffer, Cmd.Offset);
            break;
        }

        case ERHICommandType::CopyBuffer:
        {
            const auto&            Cmd         = static_cast<const FRHICommand_CopyBuffer&>(Command);
            VkBuffer SrcBuffer = Cmd.SrcBuffer.GetHandle().Cast<VkBuffer>();
            vk::Buffer             VkSrcBuffer = vk::Buffer(SrcBuffer);
            VkBuffer DstBuffer = Cmd.DstBuffer.GetHandle().Cast<VkBuffer>();
            vk::Buffer             VkDstBuffer = vk::Buffer(DstBuffer);
            TArray<vk::BufferCopy> VkRegions;
            VkRegions.Reserve(Cmd.Regions.Size());
            for (const auto& Region : Cmd.Regions)
            {
                VkRegions.Add(vk::BufferCopy(Region.SrcOffset, Region.DstOffset, Region.Size));
            }
            VkCmdBuffer.copyBuffer(VkSrcBuffer, VkDstBuffer,
                                   vk::ArrayProxy<const vk::BufferCopy>(VkRegions.Size(), VkRegions.Data()));
            break;
        }

        case ERHICommandType::SetViewport:
        {
            const auto&          Cmd = static_cast<const FRHICommand_SetViewport&>(Command);
            TArray<vk::Viewport> VkViewports;
            VkViewports.Reserve(Cmd.Viewports.Size());
            for (const auto& Viewport : Cmd.Viewports)
            {
                VkViewports.Add(vk::Viewport(Viewport.X, Viewport.Y, Viewport.Width, Viewport.Height, Viewport.MinDepth,
                                             Viewport.MaxDepth));
            }
            VkCmdBuffer.setViewport(Cmd.FirstViewport,
                                    vk::ArrayProxy<const vk::Viewport>(VkViewports.Size(), VkViewports.Data()));
            break;
        }

        case ERHICommandType::SetScissor:
        {
            const auto&        Cmd = static_cast<const FRHICommand_SetScissor&>(Command);
            TArray<vk::Rect2D> VkScissors;
            VkScissors.Reserve(Cmd.Scissors.Size());
            for (const auto& Scissor : Cmd.Scissors)
            {
                VkScissors.Add(
                    vk::Rect2D(vk::Offset2D(Scissor.X, Scissor.Y), vk::Extent2D(Scissor.Width, Scissor.Height)));
            }
            VkCmdBuffer.setScissor(Cmd.FirstScissor,
                                   vk::ArrayProxy<const vk::Rect2D>(VkScissors.Size(), VkScissors.Data()));
            break;
        }

        case ERHICommandType::PushConstants:
        {
            const auto&          Cmd        = static_cast<const FRHICommand_PushConstants&>(Command);
            VkPipelineLayout Layout = Cmd.Layout.GetHandle().Cast<VkPipelineLayout>();
            vk::PipelineLayout   VkLayout   = vk::PipelineLayout(Layout);
            vk::ShaderStageFlags StageFlags = ConvertShaderStageFlags(static_cast<ERHIShaderStage>(Cmd.StageFlags));
            VkCmdBuffer.pushConstants(VkLayout, StageFlags, Cmd.Offset, Cmd.Size, Cmd.Data.Data());
            break;
        }

        case ERHICommandType::PipelineBarrier:
        {
            const auto& Cmd = static_cast<const FRHICommand_PipelineBarrier&>(Command);
            // 转换内存屏障
            TArray<vk::MemoryBarrier> VkMemoryBarriers;
            VkMemoryBarriers.Reserve(Cmd.MemoryBarriers.Size());
            for (const auto& Barrier : Cmd.MemoryBarriers)
            {
                VkMemoryBarriers.Add(vk::MemoryBarrier(static_cast<vk::AccessFlags>(Barrier.SrcAccessMask),
                                                       static_cast<vk::AccessFlags>(Barrier.DstAccessMask)));
            }

            // 转换缓冲区内存屏障
            TArray<vk::BufferMemoryBarrier> VkBufferBarriers;
            VkBufferBarriers.Reserve(Cmd.BufferMemoryBarriers.Size());
            for (const auto& Barrier : Cmd.BufferMemoryBarriers)
            {
                VkBuffer Buffer = Barrier.Buffer.GetHandle().Cast<VkBuffer>();
                vk::Buffer VkBuffer = vk::Buffer(Buffer);
                VkBufferBarriers.Add(vk::BufferMemoryBarrier(static_cast<vk::AccessFlags>(Barrier.SrcAccessMask),
                                                             static_cast<vk::AccessFlags>(Barrier.DstAccessMask),
                                                             VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, VkBuffer,
                                                             Barrier.Offset,
                                                             Barrier.Size == 0 ? VK_WHOLE_SIZE : Barrier.Size));
            }

            // 转换图像内存屏障
            TArray<vk::ImageMemoryBarrier> VkImageBarriers;
            VkImageBarriers.Reserve(Cmd.ImageMemoryBarriers.Size());
            for (const auto& Barrier : Cmd.ImageMemoryBarriers)
            {
                VkImage Image = Barrier.Image.GetHandle().Cast<VkImage>();
                vk::Image            VkImage    = vk::Image(Image);
                vk::ImageAspectFlags AspectMask = vk::ImageAspectFlags();
                if (HasFlag(Barrier.SubresourceRange.AspectMask, ERHIImageAspect::Color))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eColor;
                }
                if (HasFlag(Barrier.SubresourceRange.AspectMask, ERHIImageAspect::Depth))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eDepth;
                }
                if (HasFlag(Barrier.SubresourceRange.AspectMask, ERHIImageAspect::Stencil))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eStencil;
                }
                vk::ImageSubresourceRange VkRange(
                    AspectMask, Barrier.SubresourceRange.BaseMipLevel, Barrier.SubresourceRange.LevelCount,
                    Barrier.SubresourceRange.BaseArrayLayer, Barrier.SubresourceRange.LayerCount);
                VkImageBarriers.Add(
                    vk::ImageMemoryBarrier(static_cast<vk::AccessFlags>(Barrier.SrcAccessMask),
                                           static_cast<vk::AccessFlags>(Barrier.DstAccessMask),
                                           ConvertImageLayout(Barrier.OldLayout), ConvertImageLayout(Barrier.NewLayout),
                                           VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, VkImage, VkRange));
            }

            VkCmdBuffer.pipelineBarrier(
                static_cast<vk::PipelineStageFlags>(Cmd.SrcStageMask),
                static_cast<vk::PipelineStageFlags>(Cmd.DstStageMask),
                static_cast<vk::DependencyFlags>(Cmd.DependencyFlags),
                vk::ArrayProxy<const vk::MemoryBarrier>(VkMemoryBarriers.Size(), VkMemoryBarriers.Data()),
                vk::ArrayProxy<const vk::BufferMemoryBarrier>(VkBufferBarriers.Size(), VkBufferBarriers.Data()),
                vk::ArrayProxy<const vk::ImageMemoryBarrier>(VkImageBarriers.Size(), VkImageBarriers.Data()));
            break;
        }

        case ERHICommandType::CopyImage:
        {
            const auto&           Cmd        = static_cast<const FRHICommand_CopyImage&>(Command);
            VkImage SrcImage = Cmd.SrcImage.GetHandle().Cast<VkImage>();
            vk::Image             VkSrcImage = vk::Image(SrcImage);
            VkImage DstImage = Cmd.DstImage.GetHandle().Cast<VkImage>();
            vk::Image             VkDstImage = vk::Image(DstImage);
            TArray<vk::ImageCopy> VkRegions;
            VkRegions.Reserve(Cmd.Regions.Size());
            for (const auto& Region : Cmd.Regions)
            {
                // 转换源子资源层
                vk::ImageAspectFlags SrcAspectMask = vk::ImageAspectFlags();
                if (HasFlag(Region.SrcSubresource.AspectMask, ERHIImageAspect::Color))
                {
                    SrcAspectMask |= vk::ImageAspectFlagBits::eColor;
                }
                if (HasFlag(Region.SrcSubresource.AspectMask, ERHIImageAspect::Depth))
                {
                    SrcAspectMask |= vk::ImageAspectFlagBits::eDepth;
                }
                if (HasFlag(Region.SrcSubresource.AspectMask, ERHIImageAspect::Stencil))
                {
                    SrcAspectMask |= vk::ImageAspectFlagBits::eStencil;
                }
                vk::ImageSubresourceLayers VkSrcSubresource(SrcAspectMask, Region.SrcSubresource.MipLevel,
                                                            Region.SrcSubresource.BaseArrayLayer,
                                                            Region.SrcSubresource.LayerCount);

                // 转换目标子资源层
                vk::ImageAspectFlags DstAspectMask = vk::ImageAspectFlags();
                if (HasFlag(Region.DstSubresource.AspectMask, ERHIImageAspect::Color))
                {
                    DstAspectMask |= vk::ImageAspectFlagBits::eColor;
                }
                if (HasFlag(Region.DstSubresource.AspectMask, ERHIImageAspect::Depth))
                {
                    DstAspectMask |= vk::ImageAspectFlagBits::eDepth;
                }
                if (HasFlag(Region.DstSubresource.AspectMask, ERHIImageAspect::Stencil))
                {
                    DstAspectMask |= vk::ImageAspectFlagBits::eStencil;
                }
                vk::ImageSubresourceLayers VkDstSubresource(DstAspectMask, Region.DstSubresource.MipLevel,
                                                            Region.DstSubresource.BaseArrayLayer,
                                                            Region.DstSubresource.LayerCount);

                VkRegions.Add(vk::ImageCopy(
                    VkSrcSubresource, vk::Offset3D(Region.SrcOffset.X, Region.SrcOffset.Y, Region.SrcOffset.Z),
                    VkDstSubresource, vk::Offset3D(Region.DstOffset.X, Region.DstOffset.Y, Region.DstOffset.Z),
                    vk::Extent3D(Region.Extent.X, Region.Extent.Y, Region.Extent.Z)));
            }
            VkCmdBuffer.copyImage(VkSrcImage, vk::ImageLayout::eGeneral, VkDstImage, vk::ImageLayout::eGeneral,
                                  vk::ArrayProxy<const vk::ImageCopy>(VkRegions.Size(), VkRegions.Data()));
            break;
        }

        case ERHICommandType::CopyBufferToImage:
        {
            const auto&                 Cmd      = static_cast<const FRHICommand_CopyBufferToImage&>(Command);
            VkBuffer Buffer = Cmd.SrcBuffer.GetHandle().Cast<VkBuffer>();
            vk::Buffer                  VkBuffer = vk::Buffer(Buffer);
            VkImage Image = Cmd.DstImage.GetHandle().Cast<VkImage>();
            vk::Image                   VkImage  = vk::Image(Image);
            TArray<vk::BufferImageCopy> VkRegions;
            VkRegions.Reserve(Cmd.Regions.Size());
            for (const auto& Region : Cmd.Regions)
            {
                // 转换图像子资源层
                vk::ImageAspectFlags AspectMask = vk::ImageAspectFlags();
                if (HasFlag(Region.ImageSubresource.AspectMask, ERHIImageAspect::Color))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eColor;
                }
                if (HasFlag(Region.ImageSubresource.AspectMask, ERHIImageAspect::Depth))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eDepth;
                }
                if (HasFlag(Region.ImageSubresource.AspectMask, ERHIImageAspect::Stencil))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eStencil;
                }
                vk::ImageSubresourceLayers VkSubresource(AspectMask, Region.ImageSubresource.MipLevel,
                                                         Region.ImageSubresource.BaseArrayLayer,
                                                         Region.ImageSubresource.LayerCount);

                VkRegions.Add(vk::BufferImageCopy(
                    Region.BufferOffset, Region.BufferRowLength, Region.BufferImageHeight, VkSubresource,
                    vk::Offset3D(Region.ImageOffset.X, Region.ImageOffset.Y, Region.ImageOffset.Z),
                    vk::Extent3D(Region.ImageExtent.X, Region.ImageExtent.Y, Region.ImageExtent.Z)));
            }
            VkCmdBuffer.copyBufferToImage(
                VkBuffer, VkImage, vk::ImageLayout::eGeneral,
                vk::ArrayProxy<const vk::BufferImageCopy>(VkRegions.Size(), VkRegions.Data()));
            break;
        }

        case ERHICommandType::CopyImageToBuffer:
        {
            const auto&                 Cmd      = static_cast<const FRHICommand_CopyImageToBuffer&>(Command);
            VkImage Image = Cmd.SrcImage.GetHandle().Cast<VkImage>();
            vk::Image                   VkImage  = vk::Image(Image);
            VkBuffer Buffer = Cmd.DstBuffer.GetHandle().Cast<VkBuffer>();
            vk::Buffer                  VkBuffer = vk::Buffer(Buffer);
            TArray<vk::BufferImageCopy> VkRegions;
            VkRegions.Reserve(Cmd.Regions.Size());
            for (const auto& Region : Cmd.Regions)
            {
                // 转换图像子资源层
                vk::ImageAspectFlags AspectMask = vk::ImageAspectFlags();
                if (HasFlag(Region.ImageSubresource.AspectMask, ERHIImageAspect::Color))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eColor;
                }
                if (HasFlag(Region.ImageSubresource.AspectMask, ERHIImageAspect::Depth))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eDepth;
                }
                if (HasFlag(Region.ImageSubresource.AspectMask, ERHIImageAspect::Stencil))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eStencil;
                }
                vk::ImageSubresourceLayers VkSubresource(AspectMask, Region.ImageSubresource.MipLevel,
                                                         Region.ImageSubresource.BaseArrayLayer,
                                                         Region.ImageSubresource.LayerCount);

                VkRegions.Add(vk::BufferImageCopy(
                    Region.BufferOffset, Region.BufferRowLength, Region.BufferImageHeight, VkSubresource,
                    vk::Offset3D(Region.ImageOffset.X, Region.ImageOffset.Y, Region.ImageOffset.Z),
                    vk::Extent3D(Region.ImageExtent.X, Region.ImageExtent.Y, Region.ImageExtent.Z)));
            }
            VkCmdBuffer.copyImageToBuffer(
                VkImage, vk::ImageLayout::eGeneral, VkBuffer,
                vk::ArrayProxy<const vk::BufferImageCopy>(VkRegions.Size(), VkRegions.Data()));
            break;
        }

        case ERHICommandType::ClearColorImage:
        {
            const auto&                       Cmd     = static_cast<const FRHICommand_ClearColorImage&>(Command);
            VkImage Image = Cmd.Image.GetHandle().Cast<VkImage>();
            vk::Image                         VkImage = vk::Image(Image);
            vk::ClearColorValue               VkColor;
            VkColor.float32[0] = Cmd.Color.X;
            VkColor.float32[1] = Cmd.Color.Y;
            VkColor.float32[2] = Cmd.Color.Z;
            VkColor.float32[3] = Cmd.Color.W;
            TArray<vk::ImageSubresourceRange> VkRanges;
            VkRanges.Reserve(Cmd.Ranges.Size());
            for (const auto& Range : Cmd.Ranges)
            {
                vk::ImageAspectFlags AspectMask = vk::ImageAspectFlags();
                if (HasFlag(Range.AspectMask, ERHIImageAspect::Color))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eColor;
                }
                if (HasFlag(Range.AspectMask, ERHIImageAspect::Depth))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eDepth;
                }
                if (HasFlag(Range.AspectMask, ERHIImageAspect::Stencil))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eStencil;
                }
                VkRanges.Add(vk::ImageSubresourceRange(AspectMask, Range.BaseMipLevel, Range.LevelCount,
                                                       Range.BaseArrayLayer, Range.LayerCount));
            }
            VkCmdBuffer.clearColorImage(
                VkImage, vk::ImageLayout::eGeneral, VkColor,
                vk::ArrayProxy<const vk::ImageSubresourceRange>(VkRanges.Size(), VkRanges.Data()));
            break;
        }

        case ERHICommandType::ClearDepthStencilImage:
        {
            const auto&                       Cmd     = static_cast<const FRHICommand_ClearDepthStencilImage&>(Command);
            VkImage Image = Cmd.Image.GetHandle().Cast<VkImage>();
            vk::Image                         VkImage = vk::Image(Image);
            vk::ClearDepthStencilValue        VkDepthStencil(Cmd.Depth, Cmd.Stencil);
            TArray<vk::ImageSubresourceRange> VkRanges;
            VkRanges.Reserve(Cmd.Ranges.Size());
            for (const auto& Range : Cmd.Ranges)
            {
                vk::ImageAspectFlags AspectMask = vk::ImageAspectFlags();
                if (HasFlag(Range.AspectMask, ERHIImageAspect::Color))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eColor;
                }
                if (HasFlag(Range.AspectMask, ERHIImageAspect::Depth))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eDepth;
                }
                if (HasFlag(Range.AspectMask, ERHIImageAspect::Stencil))
                {
                    AspectMask |= vk::ImageAspectFlagBits::eStencil;
                }
                VkRanges.Add(vk::ImageSubresourceRange(AspectMask, Range.BaseMipLevel, Range.LevelCount,
                                                       Range.BaseArrayLayer, Range.LayerCount));
            }
            VkCmdBuffer.clearDepthStencilImage(
                VkImage, vk::ImageLayout::eGeneral, VkDepthStencil,
                vk::ArrayProxy<const vk::ImageSubresourceRange>(VkRanges.Size(), VkRanges.Data()));
            break;
        }

        case ERHICommandType::BeginRendering:
        {
            const auto& Cmd = static_cast<const FRHICommand_BeginRendering&>(Command);
            // 转换颜色附件
            TArray<vk::RenderingAttachmentInfo> VkColorAttachments;
            VkColorAttachments.Reserve(Cmd.ColorAttachments.Size());
            for (const auto& Attachment : Cmd.ColorAttachments)
            {
                if (Attachment.IsValid())
                {
                    VkImageView ImageView = Attachment.GetHandle().Cast<VkImageView>();
                    vk::ImageView VkImageView = vk::ImageView(ImageView);
                    VkColorAttachments.Add(vk::RenderingAttachmentInfo(
                        VkImageView, vk::ImageLayout::eColorAttachmentOptimal, vk::ResolveModeFlagBits::eNone, nullptr,
                        vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore));
                }
            }

            // 转换深度附件
            vk::RenderingAttachmentInfo* VkDepthAttachment = nullptr;
            vk::RenderingAttachmentInfo  VkDepthAttachmentInfo;
            if (Cmd.DepthAttachment.IsValid())
            {
                VkImageView ImageView = Cmd.DepthAttachment.GetHandle().Cast<VkImageView>();
                vk::ImageView VkImageView = vk::ImageView(ImageView);
                VkDepthAttachmentInfo     = vk::RenderingAttachmentInfo(
                    VkImageView, ConvertImageLayout(Cmd.DepthAttachmentLayout), vk::ResolveModeFlagBits::eNone, nullptr,
                    vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore);
                VkDepthAttachment = &VkDepthAttachmentInfo;
            }

            // 转换模板附件
            vk::RenderingAttachmentInfo* VkStencilAttachment = nullptr;
            vk::RenderingAttachmentInfo  VkStencilAttachmentInfo;
            if (Cmd.StencilAttachment.IsValid())
            {
                VkImageView ImageView = Cmd.StencilAttachment.GetHandle().Cast<VkImageView>();
                vk::ImageView VkImageView = vk::ImageView(ImageView);
                VkStencilAttachmentInfo   = vk::RenderingAttachmentInfo(
                    VkImageView, ConvertImageLayout(Cmd.StencilAttachmentLayout), vk::ResolveModeFlagBits::eNone,
                    nullptr, vk::ImageLayout::eUndefined, vk::AttachmentLoadOp::eLoad, vk::AttachmentStoreOp::eStore);
                VkStencilAttachment = &VkStencilAttachmentInfo;
            }

            vk::RenderingInfo RenderingInfo(
                vk::RenderingFlags(),
                vk::Rect2D(vk::Offset2D(Cmd.RenderArea.X, Cmd.RenderArea.Y),
                           vk::Extent2D(Cmd.RenderArea.Width, Cmd.RenderArea.Height)),
                1, 0, VkColorAttachments.Size(), VkColorAttachments.Data(), VkDepthAttachment, VkStencilAttachment);

            VkCmdBuffer.beginRendering(RenderingInfo);
            break;
        }

        case ERHICommandType::EndRendering:
        {
            VkCmdBuffer.endRendering();
            break;
        }

        default:
        {
            HK_LOG_WARN(ELogcat::RHI, "Unimplemented command type: {}", static_cast<UInt32>(Command.CommandType));
            break;
        }
    }
}

// ============================================================================
// 辅助函数：转换命令缓冲区使用标志
// ============================================================================

vk::CommandBufferUsageFlags FGfxDeviceVk::ConvertCommandBufferUsageFlags(ERHICommandBufferUsageFlag Flags)
{
    vk::CommandBufferUsageFlags VkFlags = vk::CommandBufferUsageFlags();
    if (HasFlag(Flags, ERHICommandBufferUsageFlag::OneTimeSubmit))
    {
        VkFlags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    }
    if (HasFlag(Flags, ERHICommandBufferUsageFlag::RenderPassContinue))
    {
        VkFlags |= vk::CommandBufferUsageFlagBits::eRenderPassContinue;
    }
    if (HasFlag(Flags, ERHICommandBufferUsageFlag::SimultaneousUse))
    {
        VkFlags |= vk::CommandBufferUsageFlagBits::eSimultaneousUse;
    }
    return VkFlags;
}
