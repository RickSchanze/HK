//
// Created by Admin on 2025/12/23.
//

#include "AssetRegistry.h"
#include "Core/Logging/Logger.h"
#include "Core/Serialization/JsonArchive.h"
#include "Core/Utility/FileUtility.h"
#include <algorithm>
#include <filesystem>

// 防止 Windows 宏污染
#ifdef SearchPath
#undef SearchPath
#endif

// ============================================================================
// 静态辅助函数
// ============================================================================

/**
 * @brief 路径标准化辅助函数
 * 统一将反斜杠替换为正斜杠，确保缓存 Key 的一致性
 */
static FString NormalizeAssetPath(const FString& InPath)
{
    FString Path = InPath;
    Path.ReplaceInPlace('\\', '/');
    // 如果有需要，也可以在这里做 Trim 或 ToLower 处理
    // Path.TrimInPlace();
    return Path;
}

// ============================================================================
// FAssetRegistry 实现
// ============================================================================

FAssetMetaData& FAssetRegistry::LoadAssetMetadata(const FString& InPath)
{
    // 1. 路径标准化
    FString Path = NormalizeAssetPath(InPath);

    // 2. 检查内存缓存
    if (const auto* UuidPtr = PathToUuid.Find(Path); UuidPtr != nullptr)
    {
        if (auto* Cached = CachedMetadata.Find(*UuidPtr); Cached != nullptr)
        {
            return *Cached;
        }
    }

    // 构建 .meta 文件路径
    FString MetaPath = Path + ".meta";

    // 定义辅助 Lambda：创建默认元数据并缓存
    auto CreateAndCacheDefault = [&](const FString& NormalizedPath, const FUuid& InUuid = FUuid()) -> FAssetMetaData&
    {
        FAssetMetaData DefaultMetadata;
        DefaultMetadata.Path = NormalizedPath; // 使用标准化后的路径
        DefaultMetadata.Uuid = InUuid.IsValid() ? InUuid : FUuid::New();
        DefaultMetadata.FileType = InferFileTypeFromPath(NormalizedPath);

        CachedMetadata.Add(DefaultMetadata.Uuid, DefaultMetadata);
        UuidToPath[DefaultMetadata.Uuid] = NormalizedPath;
        PathToUuid[NormalizedPath] = DefaultMetadata.Uuid;

        return *CachedMetadata.Find(DefaultMetadata.Uuid);
    };

    // 3. 检查文件是否存在
    if (!FFileUtility::FileExists(MetaPath))
    {
        HK_LOG_WARN(ELogcat::Asset, "Metadata file not found: {}", MetaPath);
        return CreateAndCacheDefault(Path);
    }

    // 4. 打开文件
    const auto File = FFileUtility::OpenFileStream(MetaPath);
    if (!File)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to open metadata file: {}", MetaPath);
        return CreateAndCacheDefault(Path);
    }

    // 5. 反序列化
    FAssetMetaData Metadata;
    try
    {
        FJsonInputArchive Archive(*File);
        Metadata.Serialize(Archive);

        // 强行修正路径为当前标准化路径（防止 Meta 文件被移动后内部路径未更新）
        Metadata.Path = Path;

        // 如果文件类型未知，重新推断
        if (Metadata.FileType == EAssetFileType::Unknown)
        {
            Metadata.FileType = InferFileTypeFromPath(Path);
        }

        // 确保 UUID 有效
        if (!Metadata.Uuid.IsValid())
        {
             Metadata.Uuid = FUuid::New();
        }

        // 更新缓存
        CachedMetadata.Add(Metadata.Uuid, Metadata);
        UuidToPath[Metadata.Uuid] = Path;
        PathToUuid[Path] = Metadata.Uuid;

        HK_LOG_INFO(ELogcat::Asset, "Loaded metadata from: {}", MetaPath);
        return *CachedMetadata.Find(Metadata.Uuid);
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to deserialize metadata from {}: {}", MetaPath, e.what());
        // 尝试保留已读取到的 UUID，否则生成新的
        return CreateAndCacheDefault(Path, Metadata.Uuid);
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
        // PathPtr 指向的应该已经是标准化路径（因为只有 Load/Save 成功才会写入 Map）
        return LoadAssetMetadata(*PathPtr);
    }

    HK_LOG_WARN(ELogcat::Asset, "UUID not found in registry: {}", Uuid.ToString());

    // 创建临时的空元数据防止崩溃
    FAssetMetaData Metadata;
    Metadata.Uuid = Uuid;
    CachedMetadata.Add(Metadata.Uuid, Metadata);
    return *CachedMetadata.Find(Metadata.Uuid);
}

bool FAssetRegistry::SaveAssetMetadata(const FAssetMetaData& Metadata)
{
    if (!Metadata.Uuid.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Cannot save metadata with invalid UUID");
        return false;
    }

    if (Metadata.Path.IsEmpty())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Cannot save metadata with empty path");
        return false;
    }

    // 在保存前也确保路径是标准化的
    FString NormalizedPath = NormalizeAssetPath(Metadata.Path);

    // 构建 .meta 文件路径
    FString MetaPath = NormalizedPath + ".meta";

    // 创建文件流
    const auto File = FFileUtility::CreateFileStream(MetaPath, true, true);
    if (!File)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create metadata file stream: {}", MetaPath);
        return false;
    }

    try
    {
        // 如果传入的 Metadata 路径未标准化，我们这里修正它再保存
        // 注意：这里修改了传入对象的副本或者需要强转，
        // 为了安全起见，我们构造一个新的 Metadata 对象或修改 const_cast（如果业务允许）
        // 这里选择修改 const_cast，因为我们希望内存中的对象也是标准化的
        FAssetMetaData& MutableMeta = const_cast<FAssetMetaData&>(Metadata);
        if (MutableMeta.Path != NormalizedPath)
        {
            MutableMeta.Path = NormalizedPath;
        }

        FJsonOutputArchive Archive(*File);
        MutableMeta.Serialize(Archive);

        // 更新缓存
        CachedMetadata.Add(MutableMeta.Uuid, MutableMeta);
        UuidToPath[MutableMeta.Uuid] = MutableMeta.Path;
        PathToUuid[MutableMeta.Path] = MutableMeta.Uuid;

        HK_LOG_INFO(ELogcat::Asset, "Saved metadata to: {}", MetaPath);
        return true;
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to save metadata to {}: {}", MetaPath, e.what());
        return false;
    }
}

bool FAssetRegistry::SaveAssetMetadata(const FString& InPath)
{
    // 1. 路径标准化
    FString LookupPath = NormalizeAssetPath(InPath);

    // 辅助 Lambda
    auto TrySaveByKey = [&](const FString& Key) -> bool
    {
        if (const auto* UuidPtr = PathToUuid.Find(Key))
        {
            if (const auto* Cached = CachedMetadata.Find(*UuidPtr))
            {
                return SaveAssetMetadata(*Cached);
            }
        }
        return false;
    };

    // --- 策略 A: 精确匹配 ---
    if (TrySaveByKey(LookupPath))
    {
        return true;
    }

    // --- 策略 B: 处理传入的是 .meta 路径的情况 ---
    if (LookupPath.EndsWith(".meta"))
    {
        const FStringView AssetPathView = LookupPath.SubStr(0, LookupPath.Length() - 5);
        // 需要构造 FString 来查找 Map
        if (TrySaveByKey(FString(AssetPathView)))
        {
            return true;
        }
    }

    // --- 策略 C: 模糊匹配 (处理缺少扩展名的情况) ---
    bool bHasExtension = false;
    size_t LastDot = LookupPath.FindLastOf('.');
    size_t LastSlash = LookupPath.FindLastOf('/');

    if (LastDot != std::string::npos)
    {
        if (LastSlash == std::string::npos || LastDot > LastSlash)
        {
            bHasExtension = true;
        }
    }

    if (!bHasExtension)
    {
        for (const auto& [RegisteredPath, Uuid] : PathToUuid)
        {
            // 1. 必须以 LookupPath 开头
            if (!RegisteredPath.StartsWith(LookupPath)) continue;

            // 2. 检查 RegisteredPath 的点的位置
            size_t RegDotPos = RegisteredPath.FindLastOf('.');
            if (RegDotPos == std::string::npos) continue;

            // 3. 长度匹配
            if (LookupPath.Length() == RegDotPos)
            {
                if (const auto* Cached = CachedMetadata.Find(Uuid))
                {
                    HK_LOG_INFO(ELogcat::Asset, "Fuzzy match found for saving: '{}' -> '{}'", LookupPath, RegisteredPath);
                    return SaveAssetMetadata(*Cached);
                }
            }
        }
    }

    HK_LOG_ERROR(ELogcat::Asset, "Metadata not found in cache for path (or variants): {}", InPath);
    return false;
}

bool FAssetRegistry::SaveAssetMetadata(const FUuid& Uuid)
{
    if (const auto* Cached = CachedMetadata.Find(Uuid); Cached != nullptr)
    {
        return SaveAssetMetadata(*Cached);
    }

    HK_LOG_ERROR(ELogcat::Asset, "Metadata not found in cache for UUID: {}", Uuid.ToString());
    return false;
}

EAssetFileType FAssetRegistry::InferFileTypeFromPath(const FStringView& Path)
{
    if (Path.IsEmpty()) return EAssetFileType::Unknown;

    const auto DotPos = Path.FindLastOf('.'); // 使用 FStringView/FString 的 FindLastOf
    if (DotPos == std::string::npos) return EAssetFileType::Unknown;

    const auto SlashPos = Path.FindLastOf('/');
    const auto BackSlashPos = Path.FindLastOf('\\');

    size_t LastSepPos = std::string::npos;
    if (SlashPos != std::string::npos) LastSepPos = SlashPos;

    if (BackSlashPos != std::string::npos)
    {
        if (LastSepPos == std::string::npos || BackSlashPos > LastSepPos)
        {
            LastSepPos = BackSlashPos;
        }
    }

    // 如果点在目录分隔符之前
    if (LastSepPos != std::string::npos && DotPos < LastSepPos)
    {
        return EAssetFileType::Unknown;
    }

    const FStringView Extension = Path.SubStr(DotPos + 1);
    return InferFileTypeFromExtension(Extension);
}

EAssetFileType FAssetRegistry::InferFileTypeFromExtension(const FStringView& Extension)
{
    if (Extension.IsEmpty()) return EAssetFileType::Unknown;

    FString LowerExt(Extension);
    std::ranges::transform(LowerExt, LowerExt.begin(),
                           [](const unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (!LowerExt.IsEmpty() && LowerExt[0] == '.')
    {
        const FStringView ExtView = LowerExt.SubStr(1);
        LowerExt = FString(ExtView);
    }

    const FStringView LowerExtView(LowerExt);

    // 图像
    if (LowerExtView == "png") return EAssetFileType::PNG;
    if (LowerExtView == "jpg" || LowerExtView == "jpeg") return EAssetFileType::JPG;
    if (LowerExtView == "bmp") return EAssetFileType::BMP;
    if (LowerExtView == "tga") return EAssetFileType::TGA;
    if (LowerExtView == "hdr") return EAssetFileType::HDR;
    if (LowerExtView == "exr") return EAssetFileType::EXR;
    if (LowerExtView == "dds") return EAssetFileType::DDS;
    if (LowerExtView == "ktx") return EAssetFileType::KTX;
    if (LowerExtView == "ktx2") return EAssetFileType::KTX2;

    // 模型
    if (LowerExtView == "fbx") return EAssetFileType::FBX;
    if (LowerExtView == "obj") return EAssetFileType::OBJ;
    if (LowerExtView == "gltf") return EAssetFileType::GLTF;
    if (LowerExtView == "glb") return EAssetFileType::GLB;
    if (LowerExtView == "dae") return EAssetFileType::DAE;
    if (LowerExtView == "blend") return EAssetFileType::BLEND;
    if (LowerExtView == "x3d") return EAssetFileType::X3D;

    // 音频
    if (LowerExtView == "wav") return EAssetFileType::WAV;
    if (LowerExtView == "ogg") return EAssetFileType::OGG;
    if (LowerExtView == "mp3") return EAssetFileType::MP3;
    if (LowerExtView == "flac") return EAssetFileType::FLAC;

    // 视频
    if (LowerExtView == "mp4") return EAssetFileType::MP4;
    if (LowerExtView == "avi") return EAssetFileType::AVI;
    if (LowerExtView == "mov") return EAssetFileType::MOV;

    // Shader
    if (LowerExtView == "hlsl") return EAssetFileType::HLSL;
    if (LowerExtView == "glsl") return EAssetFileType::GLSL;
    if (LowerExtView == "slang") return EAssetFileType::SLANG;
    if (LowerExtView == "spirv") return EAssetFileType::SPIRV;

    // 其他
    if (LowerExtView == "json") return EAssetFileType::JSON;
    if (LowerExtView == "xml") return EAssetFileType::XML;
    if (LowerExtView == "txt") return EAssetFileType::TXT;

    return EAssetFileType::Unknown;
}