//
// Created by Admin on 2026/1/4.
//

#include "MeshLoader.h"
#include "Core/Logging/Logger.h"
#include "Core/Serialization/BinaryArchive.h"
#include "Core/Utility/FileUtility.h"
#include "Object/AssetImporter.h"
#include "Object/AssetRegistry.h"
#include "Object/AssetUtility.h"
#include "Object/Object.h"
#include "Render/Mesh/Mesh.h"
#include "Render/Mesh/MeshImporter.h"
#include "Render/Mesh/MeshUtility.h"
#include <fstream>

namespace
{
// 从 Intermediate 文件加载 Mesh
HMesh* LoadMeshFromIntermediate(const FAssetMetadata& Metadata)
{
    // 获取中间文件路径
    FString IntermediatePath = FAssetUtility::GetMeshIntermediatePath(Metadata.Uuid);

    // 打开文件流
    auto FileStream = FFileUtility::OpenFileStream(IntermediatePath, true);
    if (!FileStream)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to open intermediate file: {}", IntermediatePath);
        return nullptr;
    }

    // 反序列化 Intermediate 数据
    FMeshIntermediate Intermediate;
    {
        FBinaryInputArchive Ar(*FileStream);
        Ar(Intermediate);
    }

    // 创建 HMesh 对象
    FObjectArray& ObjectArray = FObjectArray::GetRef();
    HMesh*        Mesh         = ObjectArray.CreateObject<HMesh>(FName(Metadata.Path));
    if (!Mesh)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create HMesh object");
        return nullptr;
    }

    // 使用 MeshUtility 创建并上传 Mesh 到 GPU
    TArray<FSubMesh>         SubMeshes;
    TArray<FRHIBuffer>        StagingBuffers;
    TArray<FRHICommandBuffer> CommandBuffers;

    if (!FMeshUtility::CreateAndUploadMeshFromIntermediate(Intermediate, SubMeshes, StagingBuffers, CommandBuffers))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create and upload mesh from intermediate data");
        return nullptr;
    }

    // 设置 Mesh 的 SubMeshes
    TArray<FSubMesh>& MeshSubMeshes = Mesh->internalGetMutableSubMeshes();
    MeshSubMeshes                   = std::move(SubMeshes);

    HK_LOG_INFO(ELogcat::Asset, "Successfully loaded mesh from intermediate: {} ({} sub-meshes)", Metadata.Path,
                MeshSubMeshes.Size());
    return Mesh;
}
} // namespace

HObject* FMeshLoader::Load(const FAssetMetadata& Metadata, FType AssetType, bool ImportIfNotExist)
{
    // 校验类型
    if (AssetType != TypeOf<HMesh>())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Invalid asset type for MeshLoader: {}", AssetType->GetName());
        return nullptr;
    }

    // 获取中间文件路径
    FString IntermediatePath = FAssetUtility::GetMeshIntermediatePath(Metadata.Uuid);

    // 校验 Hash
    TSharedPtr<FAssetMetadata> MetaPtr = MakeShared<FAssetMetadata>(Metadata);
    bool bHashValid = FAssetUtility::ValidateIntermediateHash(MetaPtr, IntermediatePath);

    if (bHashValid)
    {
        // Hash 校验通过，直接从 Intermediate 加载
        HMesh* Mesh = LoadMeshFromIntermediate(Metadata);
        if (Mesh)
        {
            return Mesh;
        }
        HK_LOG_WARN(ELogcat::Asset, "Failed to load mesh from intermediate, will try import if allowed");
    }

    // Hash 校验失败或加载失败，如果允许 Import 则进行 Import
    if (ImportIfNotExist)
    {
        // 创建 Importer 并执行 Import
        FMeshImporter Importer;
        TSharedPtr<FAssetMetadata> ImportMeta = FAssetUtility::GetOrCreateAssetMetadata(Metadata.Path);
        if (!ImportMeta)
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to get or create asset metadata for import: {}", Metadata.Path);
            return nullptr;
        }

        if (!Importer.Import(ImportMeta))
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to import mesh: {}", Metadata.Path);
            return nullptr;
        }

        // Import 完成后，从 ObjectArray 中获取准备好的 Mesh
        HMesh* Mesh = FObjectArray::GetRef().FindObjectByName<HMesh>(FName(Metadata.Path));
        if (!Mesh)
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to get mesh from ObjectArray after import: {}", Metadata.Path);
            return nullptr;
        }

        return Mesh;
    }

    HK_LOG_ERROR(ELogcat::Asset, "Failed to load mesh and ImportIfNotExist is false: {}", Metadata.Path);
    return nullptr;
}

