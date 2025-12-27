//
// Created by Admin on 2025/12/26.
//

#include "TextureImporter.h"

#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/SharedPtr.h"
#include "Object/AssetImporter.h"
#include "Object/AssetRegistry.h"
#include "Object/Object.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHIBuffer.h"
#include "RHI/RHICommandBuffer.h"
#include "RHI/RHICommandPool.h"
#include "RHI/RHIImage.h"
#include "RHI/RHIImageView.h"
#include "Render/Texture/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace
{
// 使用 stb_image 加载图像数据
struct FImageData
{
    UInt8* Data     = nullptr;
    Int32  Width    = 0;
    Int32  Height   = 0;
    Int32  Channels = 0;
    bool   bIsHDR   = false;
};

FImageData LoadImageData(const FString& FilePath)
{
    FImageData Result;

    // 检查是否为 HDR 图像
    Result.bIsHDR = stbi_is_hdr(FilePath.CStr());

    if (Result.bIsHDR)
    {
        // 加载 HDR 图像
        if (float* HDRData =
                stbi_loadf(FilePath.CStr(), &Result.Width, &Result.Height, &Result.Channels, STBI_rgb_alpha))
        {
            // 将 HDR 数据转换为 UInt8（简单转换，实际可能需要更复杂的处理）
            const size_t PixelCount = static_cast<size_t>(Result.Width) * Result.Height * 4;
            Result.Data             = static_cast<UInt8*>(malloc(PixelCount));
            for (size_t I = 0; I < PixelCount; ++I)
            {
                Result.Data[I] = static_cast<UInt8>(HDRData[I] * 255.0f);
            }
            stbi_image_free(HDRData);
        }
    }
    else
    {
        // 加载普通图像（强制转换为 RGBA）
        Result.Data     = stbi_load(FilePath.CStr(), &Result.Width, &Result.Height, &Result.Channels, STBI_rgb_alpha);
        Result.Channels = 4; // 强制转换为 RGBA
    }

    return Result;
}

void FreeImageData(FImageData& ImageData)
{
    if (ImageData.Data)
    {
        stbi_image_free(ImageData.Data);
        ImageData.Data = nullptr;
    }
}

// 将图像数据上传到 GPU
bool UploadImageDataToGPU(const FImageData& ImageData, const FRHIImage& Image, ERHIImageFormat Format)
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

    FRHIBuffer StagingBuffer = GfxDevice.CreateBuffer(StagingBufferDesc);
    if (!StagingBuffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create staging buffer for texture upload");
        return false;
    }

    // 映射 staging buffer 并复制数据
    void* MappedData = GfxDevice.MapBuffer(StagingBuffer, 0, ImageSize);
    if (!MappedData)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to map staging buffer");
        GfxDevice.DestroyBuffer(StagingBuffer);
        return false;
    }

    memcpy(MappedData, ImageData.Data, ImageSize);
    GfxDevice.UnmapBuffer(StagingBuffer);

    // 获取共享的 CommandPool
    FGlobalAssetImporter& GlobalImporter = FGlobalAssetImporter::GetRef();
    FRHICommandPool       CommandPool    = GlobalImporter.GetUploadCommandPool();
    if (!CommandPool.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Global upload command pool is not available");
        GfxDevice.DestroyBuffer(StagingBuffer);
        return false;
    }

    FRHICommandBufferDesc CmdBufferDesc;
    CmdBufferDesc.Level      = ERHICommandBufferLevel::Primary;
    CmdBufferDesc.UsageFlags = ERHICommandBufferUsageFlag::OneTimeSubmit;
    CmdBufferDesc.DebugName  = "TextureUploadCommandBuffer";

    FRHICommandBuffer CommandBuffer = GfxDevice.CreateCommandBuffer(CommandPool, CmdBufferDesc);
    if (!CommandBuffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create command buffer for texture upload");
        GfxDevice.DestroyBuffer(StagingBuffer);
        return false;
    }

    // 开始记录命令
    CommandBuffer.Begin(ERHICommandBufferUsageFlag::OneTimeSubmit);

    // 转换图像布局：Undefined -> TransferDstOptimal
    TArray<FRHIImageMemoryBarrier> ImageBarriers;
    FRHIImageMemoryBarrier         Barrier;
    Barrier.OldLayout                       = ERHIImageLayout::Undefined;
    Barrier.NewLayout                       = ERHIImageLayout::TransferDstOptimal;
    Barrier.Image                           = Image;
    Barrier.SrcAccessMask                   = 0;
    Barrier.DstAccessMask                   = 0x00000008; // VK_ACCESS_TRANSFER_WRITE_BIT
    Barrier.SubresourceRange.AspectMask     = ERHIImageAspect::Color;
    Barrier.SubresourceRange.BaseMipLevel   = 0;
    Barrier.SubresourceRange.LevelCount     = 1;
    Barrier.SubresourceRange.BaseArrayLayer = 0;
    Barrier.SubresourceRange.LayerCount     = 1;
    ImageBarriers.Add(Barrier);

    // Pipeline barrier: Top of pipe -> Transfer
    // VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT = 0x00000001
    // VK_PIPELINE_STAGE_TRANSFER_BIT = 0x00000008
    CommandBuffer.PipelineBarrier(0x00000001, // VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
                                  0x00000008, // VK_PIPELINE_STAGE_TRANSFER_BIT
                                  0, TArray<FRHIMemoryBarrier>(), TArray<FRHIBufferMemoryBarrier>(), ImageBarriers);

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

    CommandBuffer.CopyBufferToImage(StagingBuffer, Image, CopyRegions);

    // 转换图像布局：TransferDstOptimal -> ShaderReadOnlyOptimal
    ImageBarriers.Clear();
    Barrier.OldLayout     = ERHIImageLayout::TransferDstOptimal;
    Barrier.NewLayout     = ERHIImageLayout::ShaderReadOnlyOptimal;
    Barrier.SrcAccessMask = 0x00000008; // VK_ACCESS_TRANSFER_WRITE_BIT
    Barrier.DstAccessMask = 0x00000020; // VK_ACCESS_SHADER_READ_BIT
    ImageBarriers.Add(Barrier);

    // Pipeline barrier: Transfer -> Fragment shader
    // VK_PIPELINE_STAGE_TRANSFER_BIT = 0x00000008
    // VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT = 0x00000080
    CommandBuffer.PipelineBarrier(0x00000008, // VK_PIPELINE_STAGE_TRANSFER_BIT
                                  0x00000080, // VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
                                  0, TArray<FRHIMemoryBarrier>(), TArray<FRHIBufferMemoryBarrier>(), ImageBarriers);

    // 结束记录命令
    CommandBuffer.End();

    // 执行命令（立即执行模式）
    CommandBuffer.Execute();

    // 清理资源（只销毁 CommandBuffer，不销毁共享的 CommandPool）
    GfxDevice.DestroyCommandBuffer(CommandPool, CommandBuffer);
    GfxDevice.DestroyBuffer(StagingBuffer);

    return true;
}

// 创建 RHIImage
FRHIImage CreateRHIImage(const FImageData& ImageData, ERHIImageFormat Format)
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

// 创建 RHIImageView
FRHIImageView CreateRHIImageView(const FRHIImage& Image, ERHIImageFormat Format)
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
} // namespace

TSharedPtr<FAssetImportSetting> FTextureImporter::GetOrCreateImportSetting(FAssetMetaData& Metadata)
{
    // 如果已存在导入设置
    if (Metadata.ImportSetting)
    {
        // 尝试转换为 FTextureImportSetting
        if (auto TextureSetting = DynamicPointerCast<FTextureImportSetting>(Metadata.ImportSetting))
        {
            // 类型匹配，返回现有设置
            return TextureSetting;
        }
        else
        {
            // 类型不匹配，这是严重错误
            HK_LOG_FATAL(ELogcat::Asset,
                         "Import setting exists but is not FTextureImportSetting! This should never happen. Path: {}",
                         Metadata.Path);
            return nullptr;
        }
    }

    // 不存在，创建新的设置
    auto NewSetting        = MakeShared<FTextureImportSetting>();
    Metadata.ImportSetting = NewSetting;
    return NewSetting;
}

bool FTextureImporter::Import(FStringView Path, EAssetFileType FileType, EAssetImportOptions Options)
{
    // 转换路径为 FString
    FString FilePath(Path);

    // 加载 .meta 文件获取导入设置
    FAssetRegistry& AssetRegistry = FAssetRegistry::GetRef();
    FAssetMetaData& Metadata      = AssetRegistry.LoadAssetMetadata(FilePath);

    // 获取或创建导入设置
    auto TextureSetting = DynamicPointerCast<FTextureImportSetting>(GetOrCreateImportSetting(Metadata));
    if (!TextureSetting)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to get or create import setting for texture: {}", FilePath);
        return false;
    }

    // 使用导入设置中的格式，如果不存在则使用默认值
    ERHIImageFormat ImageFormat = TextureSetting->GPUFormat;

    // 加载图像数据
    FImageData ImageData = LoadImageData(FilePath);
    if (!ImageData.Data || ImageData.Width <= 0 || ImageData.Height <= 0)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to load image: {}", FilePath);
        FreeImageData(ImageData);
        return false;
    }

    HK_LOG_INFO(ELogcat::Asset, "Loaded image: {}x{} ({} channels)", ImageData.Width, ImageData.Height,
                ImageData.Channels);

    // 创建 HTexture 对象
    FObjectArray& ObjectArray = FObjectArray::GetRef();
    auto*         Texture     = ObjectArray.CreateObject<HTexture>(FName(FilePath));
    if (!Texture)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create HTexture object");
        FreeImageData(ImageData);
        return false;
    }

    // 创建 RHIImage
    FRHIImage Image = CreateRHIImage(ImageData, ImageFormat);
    if (!Image.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create RHIImage");
        FreeImageData(ImageData);
        return false;
    }

    // 上传图像数据到 GPU
    if (!UploadImageDataToGPU(ImageData, Image, ImageFormat))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to upload image data to GPU");
        GetGfxDeviceRef().DestroyImage(Image);
        FreeImageData(ImageData);
        return false;
    }

    // 创建 RHIImageView
    FRHIImageView ImageView = CreateRHIImageView(Image, ImageFormat);
    if (!ImageView.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create RHIImageView");
        GetGfxDeviceRef().DestroyImage(Image);
        FreeImageData(ImageData);
        return false;
    }

    // 设置 HTexture 的图像和图像视图，以及纹理参数
    Texture->internalSetRHIImage(Image);
    Texture->internalSetRHIImageView(ImageView);
    Texture->internalSetWidth(ImageData.Width);
    Texture->internalSetHeight(ImageData.Height);
    Texture->internalSetFormat(ImageFormat);

    // 保存元数据（包含导入设置）
    Metadata.AssetType = EAssetType::Texture;
    AssetRegistry.SaveAssetMetadata(Metadata);

    // 清理图像数据
    FreeImageData(ImageData);

    HK_LOG_INFO(ELogcat::Asset, "Successfully imported texture: {}", FilePath);
    return true;
}