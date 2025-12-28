//
// Created by Admin on 2025/12/26.
//

#include "AssetImporter.h"
#include "AssetRegistry.h"
#include "Core/Logging/Logger.h"
#include "Core/Utility/HashUtility.h"
#include "Core/Utility/FileUtility.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHICommandPool.h"
#include <fstream>

bool FAssetImporter::Import(const TSharedPtr<FAssetMetadata>& Metadata, const bool AllowIntermediateFailed)
{
    this->Metadata = Metadata;
    
    // Import 时不需要校验，直接重写即可
    BeginImport();
    if (!ProcessImport())
    {
        EndImport();
        return false;
    }
    if (!ProcessAssetIntermediate())
    {
        if (!AllowIntermediateFailed)
        {
            EndImport();
            return false;
        }
    }
    EndImport();
    return true;
}

bool FAssetImporter::Import(const FStringView Path)
{
    const TSharedPtr<FAssetMetadata> Meta = GetOrCreateAssetMetadata(Path);
    return Import(Meta);
}

TSharedPtr<FAssetMetadata> FAssetImporter::GetOrCreateAssetMetadata(const FStringView AssetPath)
{
    FAssetRegistry& AssetRegistry = FAssetRegistry::GetRef();
    if (AssetRegistry.IsAssetMetadataExist(AssetPath))
    {
        return AssetRegistry.LoadAssetMetadata(AssetPath);
    }
    return AssetRegistry.CreateAssetMetadata(AssetPath);
}

void FGlobalAssetMetadataFactory::StartUp() {}

void FGlobalAssetMetadataFactory::ShutDown() {}

// 根据文件类型推断资产类型
static EAssetType InferAssetTypeFromFileType(const EAssetFileType FileType)
{
    switch (FileType)
    {
        // 图像格式 -> Texture
        case EAssetFileType::PNG:
        case EAssetFileType::JPG:
        case EAssetFileType::JPEG:
        case EAssetFileType::BMP:
        case EAssetFileType::TGA:
        case EAssetFileType::HDR:
        case EAssetFileType::EXR:
        case EAssetFileType::DDS:
        case EAssetFileType::KTX:
        case EAssetFileType::KTX2:
            return EAssetType::Texture;

        // 3D 模型格式 -> Mesh
        case EAssetFileType::FBX:
        case EAssetFileType::OBJ:
        case EAssetFileType::GLTF:
        case EAssetFileType::GLB:
        case EAssetFileType::DAE:
        case EAssetFileType::BLEND:
        case EAssetFileType::X3D:
            return EAssetType::Mesh;

        // 着色器格式 -> Shader
        case EAssetFileType::HLSL:
        case EAssetFileType::GLSL:
        case EAssetFileType::SLANG:
        case EAssetFileType::SPIRV:
            return EAssetType::Shader;

        // 其他格式无法推断
        default:
            return EAssetType::Count;
    }
}

TSharedPtr<FAssetMetadata> FGlobalAssetMetadataFactory::CreateAssetMetadata(FStringView Path)
{
    if (Path.IsEmpty())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Cannot create asset metadata with empty path");
        return nullptr;
    }

    // 1. 推断文件类型
    const EAssetFileType FileType = FAssetRegistry::InferFileTypeFromPath(Path);
    if (FileType == EAssetFileType::Unknown)
    {
        HK_LOG_WARN(ELogcat::Asset, "Unknown file type for path: {}", Path);
        return nullptr;
    }

    // 2. 根据文件类型推断资产类型
    const EAssetType AssetType = InferAssetTypeFromFileType(FileType);
    if (AssetType == EAssetType::Count)
    {
        HK_LOG_WARN(ELogcat::Asset, "Cannot infer asset type from file type {} for path: {}",
                    static_cast<int>(FileType), Path);
        return nullptr;
    }

    // 3. 获取对应的 MetadataFactory
    const int AssetTypeIndex = static_cast<int>(AssetType);
    if (!AssetMetadataFactory[AssetTypeIndex])
    {
        HK_LOG_ERROR(ELogcat::Asset, "Asset metadata factory for asset type {} not set", static_cast<int>(AssetType));
        return nullptr;
    }

    // 4. 使用 Factory 创建 Metadata
    TSharedPtr<FAssetMetadata> Metadata = AssetMetadataFactory[AssetTypeIndex]->Create();
    if (!Metadata)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create asset metadata for asset type {}", static_cast<int>(AssetType));
        return nullptr;
    }

    // 5. 设置 Metadata 的基本信息
    Metadata->Path      = FString(Path);
    Metadata->FileType  = FileType;
    Metadata->AssetType = AssetType;
    if (!Metadata->Uuid.IsValid())
    {
        Metadata->Uuid = FUuid::New();
    }

    return Metadata;
}

bool FAssetImporter::ValidateIntermediateHash(FStringView IntermediatePath) const
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

    // 读取文件中的 Hash（文件开头的前 8 字节）
    FString IntermediatePathStr(IntermediatePath);
    std::ifstream File(IntermediatePathStr.CStr(), std::ios::binary);
    if (!File.is_open())
    {
        HK_LOG_WARN(ELogcat::Asset, "Failed to open intermediate file for hash validation: {}", IntermediatePath);
        return false;
    }

    UInt64 FileHash = 0;
    File.read(reinterpret_cast<char*>(&FileHash), sizeof(UInt64));
    File.close();

    // 如果读取失败或 Hash 为 0，说明文件格式不正确
    if (FileHash == 0)
    {
        HK_LOG_WARN(ELogcat::Asset, "Failed to read hash from intermediate file: {}", IntermediatePath);
        return false;
    }

    // 比较 Hash
    if (FileHash != Metadata->IntermediateHash)
    {
        HK_LOG_INFO(ELogcat::Asset, "Intermediate file hash mismatch for: {} (expected: {}, got: {})", IntermediatePath,
                    Metadata->IntermediateHash, FileHash);
        return false;
    }

    return true;
}
