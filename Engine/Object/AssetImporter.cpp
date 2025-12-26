//
// Created by Admin on 2025/12/26.
//

#include "AssetImporter.h"
#include "AssetRegistry.h"
#include "Core/Logging/Logger.h"

void FGlobalAssetImporter::Import(FStringView Path)
{
    if (Path.IsEmpty())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Cannot import asset with empty path");
        return;
    }

    // 推断文件类型
    const EAssetFileType FileType = FAssetRegistry::InferFileTypeFromPath(Path);
    
    if (FileType == EAssetFileType::Unknown)
    {
        HK_LOG_WARN(ELogcat::Asset, "Unknown file type for path: {}", Path);
        return;
    }

    // 根据文件类型调用相应的 importer
    switch (FileType)
    {
        // 图像格式 -> Texture Importer
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
        {
            if (TextureImporter)
            {
                HK_LOG_INFO(ELogcat::Asset, "Importing texture: {}", Path);
                TextureImporter->Import(Path, FileType);
            }
            else
            {
                HK_LOG_WARN(ELogcat::Asset, "Texture importer not set, cannot import: {}", Path);
            }
            break;
        }

        // 3D 模型格式 -> Mesh Importer
        case EAssetFileType::FBX:
        case EAssetFileType::OBJ:
        case EAssetFileType::GLTF:
        case EAssetFileType::GLB:
        case EAssetFileType::DAE:
        case EAssetFileType::BLEND:
        case EAssetFileType::X3D:
        {
            if (MeshImporter)
            {
                HK_LOG_INFO(ELogcat::Asset, "Importing mesh: {}", Path);
                MeshImporter->Import(Path, FileType);
            }
            else
            {
                HK_LOG_WARN(ELogcat::Asset, "Mesh importer not set, cannot import: {}", Path);
            }
            break;
        }

        // 其他格式暂不支持
        default:
        {
            HK_LOG_WARN(ELogcat::Asset, "File type {} not supported for import: {}", static_cast<int>(FileType), Path);
            break;
        }
    }
}
