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
#include "Object/AssetUtility.h"
#include "Object/Object.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHIBuffer.h"
#include "RHI/RHICommandBuffer.h"
#include "RHI/RHICommandPool.h"
#include "RHI/RHIImage.h"
#include "RHI/RHIImageView.h"
#include "Render/RenderContext.h"
#include "Render/Texture/Texture.h"
#include "Render/Texture/TextureUtility.h"

#define STB_IMAGE_IMPLEMENTATION
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
    ImportData->Image = FTextureUtility::CreateRHIImage(ImportData->ImageData, ImportData->ImageFormat);
    if (!ImportData->Image.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create RHIImage");
        return false;
    }

    // 使用 TextureUtility 上传纹理到 GPU
    if (!FTextureUtility::UploadTextureToGPU(ImportData->ImageData, ImportData->Image, ImportData->ImageFormat,
                                              ImportData->StagingBuffer, ImportData->CommandBuffer))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to upload texture to GPU");
        return false;
    }

    // 创建 RHIImageView
    ImportData->ImageView = FTextureUtility::CreateRHIImageView(ImportData->Image, ImportData->ImageFormat);
    if (!ImportData->ImageView.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create RHIImageView");
        return false;
    }

    // 使用 TextureUtility 设置 HTexture 的 RHI 资源
    FTextureUtility::SetTextureRHIResources(ImportData->Texture, ImportData->Image, ImportData->ImageView,
                                             ImportData->ImageData.Width, ImportData->ImageData.Height,
                                             ImportData->ImageFormat);

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
    FString IntermediatePath = FAssetUtility::GetTextureIntermediatePath(Metadata->Uuid);

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

void FTextureImporter::EndImport()
{
    if (!ImportData)
    {
        return;
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
