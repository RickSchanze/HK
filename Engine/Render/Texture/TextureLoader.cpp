//
// Created by Admin on 2026/1/4.
//

#include "TextureLoader.h"
#include "Core/Logging/Logger.h"
#include "Core/Serialization/BinaryArchive.h"
#include "Core/Utility/FileUtility.h"
#include "Object/AssetImporter.h"
#include "Object/AssetRegistry.h"
#include "Object/Object.h"
#include "Render/Texture/Texture.h"
#include "Render/Texture/TextureImporter.h"
#include "Render/Texture/TextureUtility.h"
#include <fstream>

namespace
{

// 从 Intermediate 文件加载纹理
HTexture* LoadTextureFromIntermediate(const FAssetMetadata& Metadata)
{
    // 获取中间文件路径
    FString IntermediatePath = FAssetUtility::GetTextureIntermediatePath(Metadata.Uuid);

    // 打开文件流
    auto FileStream = FFileUtility::OpenFileStream(IntermediatePath, true);
    if (!FileStream)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to open intermediate file: {}", IntermediatePath);
        return nullptr;
    }

    // 反序列化 Intermediate 数据
    FTextureIntermediate Intermediate;
    {
        FBinaryInputArchive Ar(*FileStream);
        Ar(Intermediate);
    }

    // 创建 HTexture 对象
    FObjectArray& ObjectArray = FObjectArray::GetRef();
    HTexture*     Texture      = ObjectArray.CreateObject<HTexture>(FName(Metadata.Path));
    if (!Texture)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create HTexture object");
        return nullptr;
    }

    // 使用 TextureUtility 创建并上传纹理到 GPU
    FRHIImage       Image;
    FRHIImageView   ImageView;
    FRHIBuffer      StagingBuffer;
    FRHICommandBuffer CommandBuffer;

    if (!FTextureUtility::CreateAndUploadTextureFromIntermediate(Intermediate, Image, ImageView, StagingBuffer,
                                                                   CommandBuffer))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create and upload texture from intermediate data");
        return nullptr;
    }

    // 设置纹理的 RHI 资源
    FTextureUtility::SetTextureRHIResources(Texture, Image, ImageView, Intermediate.Width, Intermediate.Height,
                                             Intermediate.Format);

    HK_LOG_INFO(ELogcat::Asset, "Successfully loaded texture from intermediate: {}", Metadata.Path);
    return Texture;
}
} // namespace

HObject* FTextureLoader::Load(const FAssetMetadata& Metadata, FType AssetType, bool ImportIfNotExist)
{
    // 校验类型
    if (AssetType != TypeOf<HTexture>())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Invalid asset type for TextureLoader: {}", AssetType->Name);
        return nullptr;
    }

    // 获取中间文件路径
    FString IntermediatePath = FAssetUtility::GetTextureIntermediatePath(Metadata.Uuid);

    // 校验 Hash
    TSharedPtr<FAssetMetadata> MetaPtr = MakeShared<FAssetMetadata>(Metadata);
    bool bHashValid = FAssetUtility::ValidateIntermediateHash(MetaPtr, IntermediatePath);

    if (bHashValid)
    {
        // Hash 校验通过，直接从 Intermediate 加载
        HTexture* Texture = LoadTextureFromIntermediate(Metadata);
        if (Texture)
        {
            return Texture;
        }
        HK_LOG_WARN(ELogcat::Asset, "Failed to load texture from intermediate, will try import if allowed");
    }

    // Hash 校验失败或加载失败，如果允许 Import 则进行 Import
    if (ImportIfNotExist)
    {
        // 创建 Importer 并执行 Import
        FTextureImporter Importer;
        TSharedPtr<FAssetMetadata> ImportMeta = FAssetUtility::GetOrCreateAssetMetadata(Metadata.Path);
        if (!ImportMeta)
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to get or create asset metadata for import: {}", Metadata.Path);
            return nullptr;
        }

        if (!Importer.Import(ImportMeta))
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to import texture: {}", Metadata.Path);
            return nullptr;
        }

        // Import 完成后，从 ObjectArray 中获取准备好的 Texture
        // 由于 Import 时已经创建了对象，我们可以通过名称查找
        HTexture* Texture = FObjectArray::GetRef().FindObjectByName<HTexture>(FName(Metadata.Path));
        if (!Texture)
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to get texture from ObjectArray after import: {}", Metadata.Path);
            return nullptr;
        }

        return Texture;
    }

    HK_LOG_ERROR(ELogcat::Asset, "Failed to load texture and ImportIfNotExist is false: {}", Metadata.Path);
    return nullptr;
}

