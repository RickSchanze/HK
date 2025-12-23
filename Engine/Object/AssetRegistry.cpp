//
// Created by Admin on 2025/12/23.
//

#include "AssetRegistry.h"
#include "Core/Serialization/JsonArchive.h"
#include "Core/Logging/Logger.h"
#include "Core/Utility/FileUtility.h"
#include <filesystem>

FAssetMetaData& FAssetRegistry::LoadAssetMetadata(const FString& Path)
{
    // 先检查路径到 UUID 的映射
    if (const auto* UuidPtr = PathToUuid.Find(Path); UuidPtr != nullptr)
    {
        // 如果找到 UUID，尝试从缓存加载
        if (auto* Cached = CachedMetadata.Find(*UuidPtr); Cached != nullptr)
        {
            return *Cached;
        }
    }
    
    // 构建 .meta 文件路径
    FString MetaPath = Path + ".meta";
    
    // 检查文件是否存在
    if (!FFileUtility::FileExists(MetaPath))
    {
        HK_LOG_WARN(ELogcat::Asset, "Metadata file not found: {}", MetaPath);
        // 创建默认的元数据并添加到缓存
        FAssetMetaData DefaultMetadata;
        DefaultMetadata.Path = Path;
        DefaultMetadata.Uuid = FUuid::New();
        CachedMetadata.Add(DefaultMetadata.Uuid, DefaultMetadata);
        UuidToPath[DefaultMetadata.Uuid] = Path;
        PathToUuid[Path] = DefaultMetadata.Uuid;
        return *CachedMetadata.Find(DefaultMetadata.Uuid);
    }
    
    // 打开文件
    const auto File = FFileUtility::OpenFileStream(MetaPath);
    if (!File)
    {
        // 创建默认的元数据并添加到缓存
        FAssetMetaData DefaultMetadata;
        DefaultMetadata.Path = Path;
        DefaultMetadata.Uuid = FUuid::New();
        CachedMetadata.Add(DefaultMetadata.Uuid, DefaultMetadata);
        UuidToPath[DefaultMetadata.Uuid] = Path;
        PathToUuid[Path] = DefaultMetadata.Uuid;
        return *CachedMetadata.Find(DefaultMetadata.Uuid);
    }
    
    // 从 JSON 反序列化
    FAssetMetaData Metadata;
    try
    {
        FJsonInputArchive Archive(*File);
        Metadata.Serialize(Archive);
        
        // 确保路径正确
        Metadata.Path = Path;
        
        // 更新缓存和映射（使用 UUID 作为键）
        if (Metadata.Uuid.IsValid())
        {
            CachedMetadata.Add(Metadata.Uuid, Metadata);
            UuidToPath[Metadata.Uuid] = Path;
            PathToUuid[Path] = Metadata.Uuid;
        }
        else
        {
            // 如果 UUID 无效，生成一个新的
            Metadata.Uuid = FUuid::New();
            CachedMetadata.Add(Metadata.Uuid, Metadata);
            UuidToPath[Metadata.Uuid] = Path;
            PathToUuid[Path] = Metadata.Uuid;
        }
        
        HK_LOG_INFO(ELogcat::Asset, "Loaded metadata from: {}", MetaPath);
        return *CachedMetadata.Find(Metadata.Uuid);
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to load metadata from {}: {}", MetaPath, e.what());
        // 创建默认的元数据并添加到缓存
        Metadata.Path = Path;
        if (!Metadata.Uuid.IsValid())
        {
            Metadata.Uuid = FUuid::New();
        }
        CachedMetadata.Add(Metadata.Uuid, Metadata);
        UuidToPath[Metadata.Uuid] = Path;
        PathToUuid[Path] = Metadata.Uuid;
        return *CachedMetadata.Find(Metadata.Uuid);
    }
}

FAssetMetaData& FAssetRegistry::LoadAssetMetadata(const FUuid Uuid)
{
    // 检查缓存
    if (auto* Cached = CachedMetadata.Find(Uuid); Cached != nullptr)
    {
        return *Cached;
    }
    
    // 从 UUID 查找路径
    if (const auto* PathPtr = UuidToPath.Find(Uuid); PathPtr != nullptr)
    {
        return LoadAssetMetadata(*PathPtr);
    }
    
    HK_LOG_WARN(ELogcat::Asset, "UUID not found in registry: {}", Uuid.ToString());
    // 创建默认的元数据并添加到缓存
    FAssetMetaData Metadata;
    Metadata.Uuid = Uuid;
    CachedMetadata.Add(Metadata.Uuid, Metadata);
    return *CachedMetadata.Find(Metadata.Uuid);
}

bool FAssetRegistry::SaveAssetMetadata(FAssetMetaData& Metadata)
{
    // 检查 Metadata 是否在缓存中
    if (!Metadata.Uuid.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Cannot save metadata with invalid UUID");
        return false;
    }
    
    // 检查缓存中是否存在
    if (auto* Cached = CachedMetadata.Find(Metadata.Uuid); Cached == nullptr)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Metadata not found in cache: {}", Metadata.Uuid.ToString());
        return false;
    }
    
    // 确保路径有效
    if (Metadata.Path.IsEmpty())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Cannot save metadata with empty path");
        return false;
    }
    
    // 构建 .meta 文件路径
    FString MetaPath = Metadata.Path + ".meta";
    
    // 创建文件流（自动创建目录）
    auto File = FFileUtility::CreateFileStream(MetaPath, true, true);
    if (!File)
    {
        return false;
    }
    
    // 序列化为 JSON
    try
    {
        FJsonOutputArchive Archive(*File);
        Metadata.Serialize(Archive);
        HK_LOG_INFO(ELogcat::Asset, "Saved metadata to: {}", MetaPath);
        return true;
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to save metadata to {}: {}", MetaPath, e.what());
        return false;
    }
}

bool FAssetRegistry::SaveAssetMetadata(FString Path)
{
    // 从路径查找 UUID
    if (auto* UuidPtr = PathToUuid.Find(Path); UuidPtr != nullptr)
    {
        // 从缓存获取 Metadata
        if (auto* Cached = CachedMetadata.Find(*UuidPtr); Cached != nullptr)
        {
            return SaveAssetMetadata(*Cached);
        }
    }
    
    HK_LOG_ERROR(ELogcat::Asset, "Metadata not found in cache for path: {}", Path);
    return false;
}

bool FAssetRegistry::SaveAssetMetadata(const FUuid Uuid)
{
    // 从缓存获取 Metadata
    if (auto* Cached = CachedMetadata.Find(Uuid); Cached != nullptr)
    {
        return SaveAssetMetadata(*Cached);
    }
    
    HK_LOG_ERROR(ELogcat::Asset, "Metadata not found in cache for UUID: {}", Uuid.ToString());
    return false;
}
