//
// Created by Admin on 2025/12/26.
//

#include "TextureImporter.h"

#include "Core/Container/Array.h"
#include "Core/Logging/Logger.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/BinaryArchive.h"
#include "Core/Serialization/MemoryStream.h"
#include "Core/String/String.h"
#include "Core/Utility/FileUtility.h"
#include "Core/Utility/HashUtility.h"
#include "Core/Utility/Profiler.h"
#include "Core/Utility/SharedPtr.h"
#include "Core/Utility/Uuid.h"
#include "Object/AssetImporter.h"
#include "Object/AssetRegistry.h"
#include "Object/Object.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHIBuffer.h"
#include "RHI/RHICommandBuffer.h"
#include "RHI/RHICommandPool.h"
#include "RHI/RHIImage.h"
#include "RHI/RHIImageView.h"
#include "Render/RenderContext.h"
#include "Render/Texture/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Object/AssetManager.h"

#include <filesystem>
#include <fstream>
#include <stb_image.h>

namespace
{
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

// 获取中间文件路径
FString GetIntermediatePath(const FUuid& Guid)
{
    return FString("Intermediate/Textures/") + Guid.ToString() + ".bin";
}
} // namespace

void FTextureImporter::BeginImport()
{
    // 分配导入数据
    ImportData = New<FImportData>();
}

bool FTextureImporter::ProcessImport()
{
    if (!Metadata)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Metadata is null in ProcessImport");
        return false;
    }

    // 获取或创建导入设置
    if (!Metadata->ImportSetting)
    {
        Metadata->ImportSetting = MakeShared<FTextureImportSetting>();
    }

    auto TextureSetting = DynamicPointerCast<FTextureImportSetting>(Metadata->ImportSetting);
    if (!TextureSetting)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to get import setting for texture: {}", Metadata->Path);
        return false;
    }

    // 使用导入设置中的格式
    ImportData->ImageFormat = TextureSetting->GPUFormat;

    // 加载图像数据
    ImportData->ImageData = LoadImageData(Metadata->Path);
    if (!ImportData->ImageData.Data || ImportData->ImageData.Width <= 0 || ImportData->ImageData.Height <= 0)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to load image: {}", Metadata->Path);
        return false;
    }

    HK_LOG_INFO(ELogcat::Asset, "Loaded image: {}x{} ({} channels)", ImportData->ImageData.Width,
                ImportData->ImageData.Height, ImportData->ImageData.Channels);

    // 创建 HTexture 对象
    FObjectArray& ObjectArray = FObjectArray::GetRef();
    ImportData->Texture       = ObjectArray.CreateObject<HTexture>(FName(Metadata->Path));
    if (!ImportData->Texture)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create HTexture object");
        return false;
    }

    // 创建 RHIImage
    ImportData->Image = CreateRHIImage(ImportData->ImageData, ImportData->ImageFormat);
    if (!ImportData->Image.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create RHIImage");
        return false;
    }

    // 计算图像数据大小
    const UInt64 ImageSize =
        static_cast<UInt64>(ImportData->ImageData.Width) * ImportData->ImageData.Height * 4; // RGBA = 4 bytes per pixel

    // 创建 staging buffer
    FGfxDevice&    GfxDevice = GetGfxDeviceRef();
    FRHIBufferDesc StagingBufferDesc;
    StagingBufferDesc.Size           = ImageSize;
    StagingBufferDesc.Usage          = ERHIBufferUsage::TransferSrc;
    StagingBufferDesc.MemoryProperty = ERHIBufferMemoryProperty::HostVisible | ERHIBufferMemoryProperty::HostCoherent;
    StagingBufferDesc.DebugName      = "TextureStagingBuffer";

    ImportData->StagingBuffer = GfxDevice.CreateBuffer(StagingBufferDesc);
    if (!ImportData->StagingBuffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create staging buffer for texture upload");
        return false;
    }

    // 映射 staging buffer 并复制数据
    void* MappedData = GfxDevice.MapBuffer(ImportData->StagingBuffer, 0, ImageSize);
    if (!MappedData)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to map staging buffer");
        return false;
    }

    memcpy(MappedData, ImportData->ImageData.Data, ImageSize);
    GfxDevice.UnmapBuffer(ImportData->StagingBuffer);

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

    ImportData->CommandBuffer = GfxDevice.CreateCommandBuffer(CommandPool, CmdBufferDesc);
    if (!ImportData->CommandBuffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create command buffer for texture upload");
        return false;
    }

    // 开始记录命令
    ImportData->CommandBuffer.Begin(ERHICommandBufferUsageFlag::OneTimeSubmit);

    // 转换图像布局：Undefined -> TransferDstOptimal
    TArray<FRHIImageMemoryBarrier> ImageBarriers;
    FRHIImageMemoryBarrier         Barrier;
    Barrier.OldLayout                       = ERHIImageLayout::Undefined;
    Barrier.NewLayout                       = ERHIImageLayout::TransferDstOptimal;
    Barrier.Image                           = ImportData->Image;
    Barrier.SrcAccessMask                   = ERHIAccessFlag::None;
    Barrier.DstAccessMask                   = ERHIAccessFlag::TransferWrite;
    Barrier.SubresourceRange.AspectMask     = ERHIImageAspect::Color;
    Barrier.SubresourceRange.BaseMipLevel   = 0;
    Barrier.SubresourceRange.LevelCount     = 1;
    Barrier.SubresourceRange.BaseArrayLayer = 0;
    Barrier.SubresourceRange.LayerCount     = 1;
    ImageBarriers.Add(Barrier);

    // Pipeline barrier: Top of pipe -> Transfer
    ImportData->CommandBuffer.PipelineBarrier(ERHIPipelineStageFlag::TopOfPipe, ERHIPipelineStageFlag::Transfer,
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
    CopyRegion.ImageExtent                     = {static_cast<Int32>(ImportData->ImageData.Width),
                                                  static_cast<Int32>(ImportData->ImageData.Height), 1};
    CopyRegions.Add(CopyRegion);

    ImportData->CommandBuffer.CopyBufferToImage(ImportData->StagingBuffer, ImportData->Image, CopyRegions);

    // 转换图像布局：TransferDstOptimal -> ShaderReadOnlyOptimal
    ImageBarriers.Clear();
    Barrier.OldLayout     = ERHIImageLayout::TransferDstOptimal;
    Barrier.NewLayout     = ERHIImageLayout::ShaderReadOnlyOptimal;
    Barrier.SrcAccessMask = ERHIAccessFlag::TransferWrite;
    Barrier.DstAccessMask = ERHIAccessFlag::ShaderRead;
    ImageBarriers.Add(Barrier);

    // Pipeline barrier: Transfer -> Fragment shader
    ImportData->CommandBuffer.PipelineBarrier(ERHIPipelineStageFlag::Transfer, ERHIPipelineStageFlag::FragmentShader,
                                              ERHIDependencyFlag::None, TArray<FRHIMemoryBarrier>(),
                                              TArray<FRHIBufferMemoryBarrier>(), ImageBarriers);

    // 结束记录命令
    ImportData->CommandBuffer.End();

    // 执行命令（立即执行模式）
    ImportData->CommandBuffer.Execute();

    // 创建 RHIImageView
    ImportData->ImageView = CreateRHIImageView(ImportData->Image, ImportData->ImageFormat);
    if (!ImportData->ImageView.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create RHIImageView");
        return false;
    }

    // 设置 HTexture 的图像和图像视图，以及纹理参数
    ImportData->Texture->internalSetRHIImage(ImportData->Image);
    ImportData->Texture->internalSetRHIImageView(ImportData->ImageView);
    ImportData->Texture->internalSetWidth(ImportData->ImageData.Width);
    ImportData->Texture->internalSetHeight(ImportData->ImageData.Height);
    ImportData->Texture->internalSetFormat(ImportData->ImageFormat);

    // 保存元数据
    Metadata->AssetType = EAssetType::Texture;
    FAssetRegistry::GetRef().SaveAssetMetadata(Metadata);

    HK_LOG_INFO(ELogcat::Asset, "Successfully processed texture import: {}", Metadata->Path);
    return true;
}

bool FTextureImporter::ProcessAssetIntermediate()
{
    if (!Metadata || !ImportData)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Metadata or ImportData is null in ProcessAssetIntermediate");
        return false;
    }

    // 获取中间文件路径
    FString IntermediatePath = GetIntermediatePath(Metadata->Uuid);

    // 构建中间数据结构（先不设置 Hash）
    FTextureIntermediate Intermediate;
    Intermediate.Width  = ImportData->ImageData.Width;
    Intermediate.Height = ImportData->ImageData.Height;
    Intermediate.Format = ImportData->ImageFormat;

    // 复制图像数据
    const UInt64 ImageSize =
        static_cast<UInt64>(ImportData->ImageData.Width) * ImportData->ImageData.Height * 4; // RGBA = 4 bytes per pixel
    Intermediate.ImageData.Resize(static_cast<size_t>(ImageSize));
    std::memcpy(Intermediate.ImageData.Data(), ImportData->ImageData.Data, ImageSize);

    // 先序列化以计算 Hash（此时 Hash 字段为 0），直接计算不存储
    FHashOutputStream HashStream;
    {
        FBinaryOutputArchive HashAr(HashStream);
        HashAr(Intermediate);
    }

    // 获取计算出的 Hash（不包括 Hash 字段本身，因为此时 Hash 为 0）
    const UInt64 Hash = HashStream.GetHash();

    // 设置 Hash 到 Intermediate
    Intermediate.Hash = Hash;

    // 创建文件流并序列化
    auto Stream = FFileUtility::CreateFileStream(IntermediatePath, true, true);
    if (!Stream)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create file stream for intermediate file: {}", IntermediatePath);
        return false;
    }

    FBinaryOutputArchive Ar(*Stream);
    Intermediate.Serialize(Ar);

    // 更新 Metadata 中的 Hash
    Metadata->IntermediateHash = Hash;
    FAssetRegistry::GetRef().SaveAssetMetadata(Metadata);

    HK_LOG_INFO(ELogcat::Asset, "Saved intermediate texture data to: {} (Hash: {})", IntermediatePath, Hash);
    return true;
}

void FTextureImporter::EndImport(bool Success)
{
    if (!ImportData)
    {
        return;
    }
    if (Success)
    {
        FAssetManager::GetRef().RegisterAsset(Metadata->Uuid, Metadata->Path, ImportData->Texture);
    }

    FGfxDevice&     GfxDevice     = GetGfxDeviceRef();
    FRenderContext& RenderContext = FRenderContext::GetRef();
    FRHICommandPool CommandPool   = RenderContext.GetUploadCommandPool();

    // 清理资源
    if (ImportData->CommandBuffer.IsValid())
    {
        GfxDevice.DestroyCommandBuffer(CommandPool, ImportData->CommandBuffer);
    }

    if (ImportData->StagingBuffer.IsValid())
    {
        GfxDevice.DestroyBuffer(ImportData->StagingBuffer);
    }

    // 清理图像数据
    FreeImageData(ImportData->ImageData);

    // 删除导入数据
    Delete(ImportData);
    ImportData = nullptr;
}
