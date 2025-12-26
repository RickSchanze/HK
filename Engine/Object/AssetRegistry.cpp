//
// Created by Admin on 2025/12/23.
//

#include "AssetRegistry.h"
#include "Core/Logging/Logger.h"
#include "Core/Serialization/JsonArchive.h"
#include "Core/Utility/FileUtility.h"
#include <algorithm>
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
        DefaultMetadata.FileType = InferFileTypeFromPath(Path);
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
        DefaultMetadata.FileType = InferFileTypeFromPath(Path);
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

        // 如果文件类型未设置，从路径推断
        if (Metadata.FileType == EAssetFileType::Unknown)
        {
            Metadata.FileType = InferFileTypeFromPath(Path);
        }

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
        if (Metadata.FileType == EAssetFileType::Unknown)
        {
            Metadata.FileType = InferFileTypeFromPath(Path);
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

bool FAssetRegistry::SaveAssetMetadata(const FAssetMetaData& Metadata)
{
    // 检查 UUID 是否有效
    if (!Metadata.Uuid.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Cannot save metadata with invalid UUID");
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
    const auto File = FFileUtility::CreateFileStream(MetaPath, true, true);
    if (!File)
    {
        return false;
    }

    // 序列化为 JSON
    try
    {
        FJsonOutputArchive Archive(*File);
        // Serialize 是模板函数，可以接受 const 对象
        const_cast<FAssetMetaData&>(Metadata).Serialize(Archive);

        // 保存成功后，更新缓存和映射
        CachedMetadata.Add(Metadata.Uuid, Metadata);
        UuidToPath[Metadata.Uuid] = Metadata.Path;
        PathToUuid[Metadata.Path] = Metadata.Uuid;

        HK_LOG_INFO(ELogcat::Asset, "Saved metadata to: {}", MetaPath);
        return true;
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to save metadata to {}: {}", MetaPath, e.what());
        return false;
    }
}

bool FAssetRegistry::SaveAssetMetadata(const FString& Path)
{
    // 从路径查找 UUID
    if (const auto* UuidPtr = PathToUuid.Find(Path); UuidPtr != nullptr)
    {
        // 从缓存获取 Metadata
        if (const auto* Cached = CachedMetadata.Find(*UuidPtr); Cached != nullptr)
        {
            return SaveAssetMetadata(*Cached);
        }
    }

    HK_LOG_ERROR(ELogcat::Asset, "Metadata not found in cache for path: {}", Path);
    return false;
}

bool FAssetRegistry::SaveAssetMetadata(const FUuid& Uuid)
{
    // 从缓存获取 Metadata
    if (const auto* Cached = CachedMetadata.Find(Uuid); Cached != nullptr)
    {
        return SaveAssetMetadata(*Cached);
    }

    HK_LOG_ERROR(ELogcat::Asset, "Metadata not found in cache for UUID: {}", Uuid.ToString());
    return false;
}

EAssetFileType FAssetRegistry::InferFileTypeFromPath(const FStringView& Path)
{
    // 查找最后一个点
    const auto DotPos = Path.FindLastOf('.');
    if (DotPos == FStringView::NPos)
    {
        return EAssetFileType::Unknown;
    }

    // 提取扩展名（不包含点）
    const FStringView Extension = Path.SubStr(DotPos + 1);
    return InferFileTypeFromExtension(Extension);
}

EAssetFileType FAssetRegistry::InferFileTypeFromExtension(const FStringView& Extension)
{
    if (Extension.IsEmpty())
    {
        return EAssetFileType::Unknown;
    }

    // 转换为小写进行比较
    FString LowerExt(Extension);
    std::ranges::transform(LowerExt, LowerExt.begin(),
                           [](const unsigned char c) { return static_cast<char>(std::tolower(c)); });

    // 移除前导点（如果有）
    if (!LowerExt.IsEmpty() && LowerExt[0] == '.')
    {
        const FStringView ExtView = LowerExt.SubStr(1);
        LowerExt = FString(ExtView);
    }

    // 使用 FStringView 进行比较
    const FStringView LowerExtView(LowerExt);

    // 图像格式
    if (LowerExtView == "png")
        return EAssetFileType::PNG;
    if (LowerExtView == "jpg" || LowerExtView == "jpeg")
        return EAssetFileType::JPG;
    if (LowerExtView == "bmp")
        return EAssetFileType::BMP;
    if (LowerExtView == "tga")
        return EAssetFileType::TGA;
    if (LowerExtView == "hdr")
        return EAssetFileType::HDR;
    if (LowerExtView == "exr")
        return EAssetFileType::EXR;
    if (LowerExtView == "dds")
        return EAssetFileType::DDS;
    if (LowerExtView == "ktx")
        return EAssetFileType::KTX;
    if (LowerExtView == "ktx2")
        return EAssetFileType::KTX2;

    // 3D 模型格式
    if (LowerExtView == "fbx")
        return EAssetFileType::FBX;
    if (LowerExtView == "obj")
        return EAssetFileType::OBJ;
    if (LowerExtView == "gltf")
        return EAssetFileType::GLTF;
    if (LowerExtView == "glb")
        return EAssetFileType::GLB;
    if (LowerExtView == "dae")
        return EAssetFileType::DAE;
    if (LowerExtView == "blend")
        return EAssetFileType::BLEND;
    if (LowerExtView == "x3d")
        return EAssetFileType::X3D;

    // 音频格式
    if (LowerExtView == "wav")
        return EAssetFileType::WAV;
    if (LowerExtView == "ogg")
        return EAssetFileType::OGG;
    if (LowerExtView == "mp3")
        return EAssetFileType::MP3;
    if (LowerExtView == "flac")
        return EAssetFileType::FLAC;

    // 视频格式
    if (LowerExtView == "mp4")
        return EAssetFileType::MP4;
    if (LowerExtView == "avi")
        return EAssetFileType::AVI;
    if (LowerExtView == "mov")
        return EAssetFileType::MOV;

    // 着色器格式
    if (LowerExtView == "hlsl")
        return EAssetFileType::HLSL;
    if (LowerExtView == "glsl")
        return EAssetFileType::GLSL;
    if (LowerExtView == "slang")
        return EAssetFileType::SLANG;
    if (LowerExtView == "spirv")
        return EAssetFileType::SPIRV;

    // 其他
    if (LowerExtView == "json")
        return EAssetFileType::JSON;
    if (LowerExtView == "xml")
        return EAssetFileType::XML;
    if (LowerExtView == "txt")
        return EAssetFileType::TXT;

    return EAssetFileType::Unknown;
}
