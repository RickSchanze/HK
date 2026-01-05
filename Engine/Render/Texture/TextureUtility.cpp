//
// Created by Admin on 2026/1/4.
//

#include "TextureUtility.h"
#include "Core/Logging/Logger.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHICommandPool.h"
#include "Render/RenderContext.h"
#include "Render/Texture/Texture.h"
#include "Render/Texture/TextureImporter.h"
#include <cstring>

FRHIImage FTextureUtility::CreateRHIImage(const FImageData& ImageData, ERHIImageFormat Format)
{
    FGfxDevice& GfxDevice = GetGfxDeviceRef();

    FRHIImageDesc ImageDesc;
    ImageDesc.Type          = ERHIImageType::Image2D;
    ImageDesc.Format        = Format;
    ImageDesc.Extent        = {ImageData.Width, ImageData.Height, 1};
    ImageDesc.MipLevels     = 1;
    ImageDesc.ArrayLayers   = 1;
    ImageDesc.Samples       = ERHISampleCount::Sample1;
    ImageDesc.Usage         = ERHIImageUsage::TransferDst | ERHIImageUsage::Sampled;
    ImageDesc.InitialLayout = ERHIImageLayout::Undefined;
    ImageDesc.DebugName     = "TextureImage";

    return GfxDevice.CreateImage(ImageDesc);
}

FRHIImageView FTextureUtility::CreateRHIImageView(const FRHIImage& Image, ERHIImageFormat Format)
{
    FGfxDevice& GfxDevice = GetGfxDeviceRef();

    FRHIImageViewDesc ViewDesc;
    ViewDesc.ViewType       = ERHIImageType::Image2D;
    ViewDesc.Format         = Format;
    ViewDesc.Aspects        = ERHIImageAspect::Color;
    ViewDesc.BaseMipLevel   = 0;
    ViewDesc.LevelCount     = 1;
    ViewDesc.BaseArrayLayer = 0;
    ViewDesc.LayerCount     = 1;
    ViewDesc.DebugName      = "TextureImageView";

    return GfxDevice.CreateImageView(Image, ViewDesc);
}

bool FTextureUtility::UploadTextureToGPU(const FImageData& ImageData, const FRHIImage& Image,
                                         ERHIImageFormat ImageFormat, FRHIBuffer& OutStagingBuffer,
                                         FRHICommandBuffer& OutCommandBuffer)
{
    FGfxDevice& GfxDevice = GetGfxDeviceRef();

    // 计算图像数据大小
    const UInt64 ImageSize = static_cast<UInt64>(ImageData.Width) * ImageData.Height * 4; // RGBA = 4 bytes per pixel

    // 创建 staging buffer
    FRHIBufferDesc StagingBufferDesc;
    StagingBufferDesc.Size           = ImageSize;
    StagingBufferDesc.Usage          = ERHIBufferUsage::TransferSrc;
    StagingBufferDesc.MemoryProperty = ERHIBufferMemoryProperty::HostVisible | ERHIBufferMemoryProperty::HostCoherent;
    StagingBufferDesc.DebugName      = "TextureStagingBuffer";

    OutStagingBuffer = GfxDevice.CreateBuffer(StagingBufferDesc);
    if (!OutStagingBuffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create staging buffer for texture upload");
        return false;
    }

    // 映射 staging buffer 并复制数据
    void* MappedData = GfxDevice.MapBuffer(OutStagingBuffer, 0, ImageSize);
    if (!MappedData)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to map staging buffer");
        return false;
    }

    memcpy(MappedData, ImageData.Data, ImageSize);
    GfxDevice.UnmapBuffer(OutStagingBuffer);

    // 获取全局 CommandPool
    FRenderContext& RenderContext = FRenderContext::GetRef();
    FRHICommandPool CommandPool   = RenderContext.GetUploadCommandPool();
    if (!CommandPool.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Global upload command pool is not available");
        return false;
    }

    // 创建命令缓冲区
    FRHICommandBufferDesc CmdBufferDesc;
    CmdBufferDesc.Level      = ERHICommandBufferLevel::Primary;
    CmdBufferDesc.UsageFlags = ERHICommandBufferUsageFlag::OneTimeSubmit;
    CmdBufferDesc.DebugName  = "TextureUploadCommandBuffer";

    OutCommandBuffer = GfxDevice.CreateCommandBuffer(CommandPool, CmdBufferDesc);
    if (!OutCommandBuffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create command buffer for texture upload");
        return false;
    }

    // 开始记录命令
    OutCommandBuffer.Begin(ERHICommandBufferUsageFlag::OneTimeSubmit);

    // 转换图像布局：Undefined -> TransferDstOptimal
    TArray<FRHIImageMemoryBarrier> ImageBarriers;
    FRHIImageMemoryBarrier         Barrier;
    Barrier.OldLayout                       = ERHIImageLayout::Undefined;
    Barrier.NewLayout                       = ERHIImageLayout::TransferDstOptimal;
    Barrier.Image                           = Image;
    Barrier.SrcAccessMask                   = ERHIAccessFlag::None;
    Barrier.DstAccessMask                   = ERHIAccessFlag::TransferWrite;
    Barrier.SubresourceRange.AspectMask     = ERHIImageAspect::Color;
    Barrier.SubresourceRange.BaseMipLevel   = 0;
    Barrier.SubresourceRange.LevelCount     = 1;
    Barrier.SubresourceRange.BaseArrayLayer = 0;
    Barrier.SubresourceRange.LayerCount     = 1;
    ImageBarriers.Add(Barrier);

    // Pipeline barrier: Top of pipe -> Transfer
    OutCommandBuffer.PipelineBarrier(ERHIPipelineStageFlag::TopOfPipe, ERHIPipelineStageFlag::Transfer,
                                     ERHIDependencyFlag::None, TArray<FRHIMemoryBarrier>(),
                                     TArray<FRHIBufferMemoryBarrier>(), ImageBarriers);

    // 复制数据从 buffer 到 image
    TArray<FRHIBufferImageCopyRegion> CopyRegions;
    FRHIBufferImageCopyRegion         CopyRegion;
    CopyRegion.BufferOffset                    = 0;
    CopyRegion.BufferRowLength                 = 0; // 紧密打包
    CopyRegion.BufferImageHeight               = 0; // 紧密打包
    CopyRegion.ImageSubresource.AspectMask     = ERHIImageAspect::Color;
    CopyRegion.ImageSubresource.MipLevel       = 0;
    CopyRegion.ImageSubresource.BaseArrayLayer = 0;
    CopyRegion.ImageSubresource.LayerCount     = 1;
    CopyRegion.ImageOffset                     = {0, 0, 0};
    CopyRegion.ImageExtent = {static_cast<Int32>(ImageData.Width), static_cast<Int32>(ImageData.Height), 1};
    CopyRegions.Add(CopyRegion);

    OutCommandBuffer.CopyBufferToImage(OutStagingBuffer, Image, CopyRegions);

    // 转换图像布局：TransferDstOptimal -> ShaderReadOnlyOptimal
    ImageBarriers.Clear();
    Barrier.OldLayout     = ERHIImageLayout::TransferDstOptimal;
    Barrier.NewLayout     = ERHIImageLayout::ShaderReadOnlyOptimal;
    Barrier.SrcAccessMask = ERHIAccessFlag::TransferWrite;
    Barrier.DstAccessMask = ERHIAccessFlag::ShaderRead;
    ImageBarriers.Add(Barrier);

    // Pipeline barrier: Transfer -> Fragment shader
    OutCommandBuffer.PipelineBarrier(ERHIPipelineStageFlag::Transfer, ERHIPipelineStageFlag::FragmentShader,
                                     ERHIDependencyFlag::None, TArray<FRHIMemoryBarrier>(),
                                     TArray<FRHIBufferMemoryBarrier>(), ImageBarriers);

    // 结束记录命令
    OutCommandBuffer.End();

    {
        FScopedRHIFence Fence;
        OutCommandBuffer.Submit({}, {}, Fence.Fence);
    }

    return true;
}

bool FTextureUtility::CreateAndUploadTextureFromIntermediate(const FTextureIntermediate& Intermediate,
                                                             FRHIImage& OutImage, FRHIImageView& OutImageView,
                                                             FRHIBuffer&        OutStagingBuffer,
                                                             FRHICommandBuffer& OutCommandBuffer)
{
    // 创建 FImageData 用于上传
    FImageData ImageData;
    ImageData.Width    = Intermediate.Width;
    ImageData.Height   = Intermediate.Height;
    ImageData.Channels = 4; // RGBA
    ImageData.bIsHDR   = false;
    ImageData.Data     = const_cast<UInt8*>(Intermediate.ImageData.Data()); // 临时使用，不会修改

    // 创建 RHIImage
    OutImage = CreateRHIImage(ImageData, Intermediate.Format);
    if (!OutImage.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create RHIImage from intermediate data");
        return false;
    }

    // 上传纹理数据到 GPU
    if (!UploadTextureToGPU(ImageData, OutImage, Intermediate.Format, OutStagingBuffer, OutCommandBuffer))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to upload texture to GPU from intermediate data");
        return false;
    }

    // 创建 RHIImageView
    OutImageView = CreateRHIImageView(OutImage, Intermediate.Format);
    if (!OutImageView.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create RHIImageView from intermediate data");
        return false;
    }

    return true;
}

void FTextureUtility::SetTextureRHIResources(HTexture* Texture, const FRHIImage& Image, const FRHIImageView& ImageView,
                                             Int32 Width, Int32 Height, ERHIImageFormat Format)
{
    if (!Texture)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Texture is null in SetTextureRHIResources");
        return;
    }

    Texture->internalSetRHIImage(Image);
    Texture->internalSetRHIImageView(ImageView);
    Texture->internalSetWidth(Width);
    Texture->internalSetHeight(Height);
    Texture->internalSetFormat(Format);
}
