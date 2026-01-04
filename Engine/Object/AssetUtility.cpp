//
// Created by Admin on 2026/1/4.
//

#include "AssetUtility.h"
#include "Core/Logging/Logger.h"
#include "Core/Utility/FileUtility.h"

TSharedPtr<FAssetMetadata> FAssetUtility::GetOrCreateAssetMetadata(FStringView AssetPath)
{
    FAssetRegistry& AssetRegistry = FAssetRegistry::GetRef();
    if (AssetRegistry.IsAssetMetadataExist(AssetPath))
    {
        return AssetRegistry.LoadAssetMetadata(AssetPath);
    }
    return AssetRegistry.CreateAssetMetadata(AssetPath);
}

bool FAssetUtility::ValidateIntermediateHash(const TSharedPtr<FAssetMetadata>& Metadata, FStringView IntermediatePath)
{
    if (!Metadata)
    {
        return false;
    }

    // 如果中间文件不存在，认为校验失败（需要重新生成）
    if (!FFileUtility::FileExists(IntermediatePath))
    {
        HK_LOG_INFO(ELogcat::Asset, "Intermediate file not found: {}", IntermediatePath);
        return false;
    }

    // 如果 Metadata 中没有 Hash，认为校验失败（需要重新生成）
    if (Metadata->IntermediateHash == 0)
    {
        HK_LOG_INFO(ELogcat::Asset, "No hash stored in metadata for: {}", IntermediatePath);
        return false;
    }

    // 使用 OpenFileStream 打开文件
    auto FileStream = FFileUtility::OpenFileStream(IntermediatePath, true);
    if (!FileStream)
    {
        HK_LOG_WARN(ELogcat::Asset, "Failed to open intermediate file for hash validation: {}", IntermediatePath);
        return false;
    }

    // 读取文件开头的 Hash（假设 Hash 是第一个字段）
    UInt64 FileHash = 0;
    FileStream->read(reinterpret_cast<char*>(&FileHash), sizeof(UInt64));

    if (!FileStream->good() || FileHash == 0)
    {
        HK_LOG_WARN(ELogcat::Asset, "Failed to read hash from intermediate file: {}", IntermediatePath);
        return false;
    }

    // 比较 Metadata 中的 Hash
    if (FileHash != Metadata->IntermediateHash)
    {
        HK_LOG_INFO(ELogcat::Asset, "Intermediate file hash mismatch for: {} (expected: {}, got: {})", IntermediatePath,
                    Metadata->IntermediateHash, FileHash);
        return false;
    }

    return true;
}

FString FAssetUtility::GetTextureIntermediatePath(const FUuid& Guid)
{
    return FString("Intermediate/Textures/") + Guid.ToString() + ".bin";
}

FString FAssetUtility::GetMeshIntermediatePath(const FUuid& Guid)
{
    return FString("Intermediate/Meshes/") + Guid.ToString() + ".bin";
}

