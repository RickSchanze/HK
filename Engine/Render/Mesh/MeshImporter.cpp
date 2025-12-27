//
// Created by Admin on 2025/12/27.
//

#include "MeshImporter.h"

#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/SharedPtr.h"
#include "Math/Vector.h"
#include "Object/AssetImporter.h"
#include "Object/AssetRegistry.h"
#include "Object/Object.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHIBuffer.h"
#include "RHI/RHICommandBuffer.h"
#include "RHI/RHICommandPool.h"
#include "Render/Mesh/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace
{
// 顶点数据结构（与 Common.slang 中的 Vertex_PNU 对应）
struct FVertexPNU
{
    FVector3f Position;
    FVector3f Normal;
    FVector2f UV;
};

// 使用 assimp 加载模型数据
struct FMeshData
{
    TArray<FVertexPNU> Vertices;
    TArray<UInt32>     Indices;
    UInt32             VertexCount = 0;
    UInt32             IndexCount  = 0;
};

// 处理单个 Mesh（从 aiMesh 提取数据）
bool ProcessMesh(const aiMesh* InMesh, FMeshData& OutMeshData)
{
    if (!InMesh)
    {
        return false;
    }

    // 检查是否有必要的属性
    if (!InMesh->HasPositions())
    {
        HK_LOG_WARN(ELogcat::Asset, "Mesh has no positions, skipping");
        return false;
    }

    const bool bHasNormals  = InMesh->HasNormals();
    const bool bHasTexCoords = InMesh->HasTextureCoords(0);

    // 提取顶点数据
    OutMeshData.Vertices.Reserve(InMesh->mNumVertices);
    for (UInt32 I = 0; I < InMesh->mNumVertices; ++I)
    {
        FVertexPNU Vertex;

        // Position
        Vertex.Position = FVector3f(InMesh->mVertices[I].x, InMesh->mVertices[I].y, InMesh->mVertices[I].z);

        // Normal（如果没有则使用默认值）
        if (bHasNormals)
        {
            Vertex.Normal = FVector3f(InMesh->mNormals[I].x, InMesh->mNormals[I].y, InMesh->mNormals[I].z);
        }
        else
        {
            Vertex.Normal = FVector3f(0.0f, 1.0f, 0.0f);
        }

        // UV（如果没有则使用默认值）
        if (bHasTexCoords)
        {
            Vertex.UV = FVector2f(InMesh->mTextureCoords[0][I].x, InMesh->mTextureCoords[0][I].y);
        }
        else
        {
            Vertex.UV = FVector2f(0.0f, 0.0f);
        }

        OutMeshData.Vertices.Add(Vertex);
    }

    // 提取索引数据
    OutMeshData.Indices.Reserve(InMesh->mNumFaces * 3); // 假设都是三角形
    for (UInt32 I = 0; I < InMesh->mNumFaces; ++I)
    {
        const aiFace& Face = InMesh->mFaces[I];
        if (Face.mNumIndices == 3)
        {
            // 三角形
            OutMeshData.Indices.Add(Face.mIndices[0]);
            OutMeshData.Indices.Add(Face.mIndices[1]);
            OutMeshData.Indices.Add(Face.mIndices[2]);
        }
        else
        {
            // 非三角形面，跳过或警告
            HK_LOG_WARN(ELogcat::Asset, "Face {} has {} indices, expected 3 (triangle)", I, Face.mNumIndices);
        }
    }

    OutMeshData.VertexCount = static_cast<UInt32>(OutMeshData.Vertices.Size());
    OutMeshData.IndexCount  = static_cast<UInt32>(OutMeshData.Indices.Size());

    return true;
}

// 递归处理场景节点
void ProcessNode(const aiNode* InNode, const aiScene* InScene, TArray<FMeshData>& OutMeshes)
{
    // 处理当前节点的所有 Mesh
    for (UInt32 I = 0; I < InNode->mNumMeshes; ++I)
    {
        const aiMesh* Mesh = InScene->mMeshes[InNode->mMeshes[I]];
        FMeshData     MeshData;
        if (ProcessMesh(Mesh, MeshData))
        {
            OutMeshes.Add(MeshData);
        }
    }

    // 递归处理子节点
    for (UInt32 I = 0; I < InNode->mNumChildren; ++I)
    {
        ProcessNode(InNode->mChildren[I], InScene, OutMeshes);
    }
}

// 将 EMeshImportFlag 转换为 assimp 的导入标志
UInt32 ConvertImportFlags(EMeshImportFlag Flags)
{
    UInt32 AssimpFlags = 0;

    if (static_cast<UInt32>(Flags & EMeshImportFlag::Triangulate))
        AssimpFlags |= aiProcess_Triangulate;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::GenNormals))
        AssimpFlags |= aiProcess_GenNormals;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::GenSmoothNormals))
        AssimpFlags |= aiProcess_GenSmoothNormals;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::SplitLargeMeshes))
        AssimpFlags |= aiProcess_SplitLargeMeshes;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::PreTransformVertices))
        AssimpFlags |= aiProcess_PreTransformVertices;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::CalcTangentSpace))
        AssimpFlags |= aiProcess_CalcTangentSpace;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::JoinIdenticalVertices))
        AssimpFlags |= aiProcess_JoinIdenticalVertices;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::RemoveRedundantMaterials))
        AssimpFlags |= aiProcess_RemoveRedundantMaterials;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::SplitByBoneCount))
        AssimpFlags |= aiProcess_SplitByBoneCount;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::FindDegenerates))
        AssimpFlags |= aiProcess_FindDegenerates;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::FindInvalidData))
        AssimpFlags |= aiProcess_FindInvalidData;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::GenUVCoords))
        AssimpFlags |= aiProcess_GenUVCoords;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::TransformUVCoords))
        AssimpFlags |= aiProcess_TransformUVCoords;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::FindInstances))
        AssimpFlags |= aiProcess_FindInstances;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::OptimizeMeshes))
        AssimpFlags |= aiProcess_OptimizeMeshes;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::OptimizeGraph))
        AssimpFlags |= aiProcess_OptimizeGraph;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::FlipUVs))
        AssimpFlags |= aiProcess_FlipUVs;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::FlipWindingOrder))
        AssimpFlags |= aiProcess_FlipWindingOrder;
    if (static_cast<UInt32>(Flags & EMeshImportFlag::SortByPType))
        AssimpFlags |= aiProcess_SortByPType;

    return AssimpFlags;
}

// 使用 assimp 加载模型
bool LoadMeshData(const FString& FilePath, EMeshImportFlag ImportFlags, TArray<FMeshData>& OutMeshes)
{
    Assimp::Importer Importer;

    // 将引擎的导入标志转换为 assimp 的标志
    const UInt32 AssimpFlags = ConvertImportFlags(ImportFlags);

    const aiScene* Scene = Importer.ReadFile(FilePath.CStr(), AssimpFlags);

    if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to load mesh: {}", Importer.GetErrorString());
        return false;
    }

    // 处理场景中的所有 Mesh
    ProcessNode(Scene->mRootNode, Scene, OutMeshes);

    if (OutMeshes.IsEmpty())
    {
        HK_LOG_ERROR(ELogcat::Asset, "No valid meshes found in file: {}", FilePath);
        return false;
    }

    HK_LOG_INFO(ELogcat::Asset, "Loaded {} sub-meshes from: {}", OutMeshes.Size(), FilePath);
    return true;
}

// 上传缓冲区数据到 GPU
bool UploadBufferDataToGPU(const void* Data, UInt64 DataSize, FRHIBuffer& DstBuffer, ERHIBufferUsage Usage,
                           const FString& DebugName)
{
    if (!Data || DataSize == 0)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Invalid buffer data for upload");
        return false;
    }

    FGfxDevice& GfxDevice = GetGfxDeviceRef();

    // 创建 staging buffer
    FRHIBufferDesc StagingBufferDesc;
    StagingBufferDesc.Size           = DataSize;
    StagingBufferDesc.Usage          = ERHIBufferUsage::TransferSrc;
    StagingBufferDesc.MemoryProperty = ERHIBufferMemoryProperty::HostVisible | ERHIBufferMemoryProperty::HostCoherent;
    StagingBufferDesc.DebugName      = DebugName + "_Staging";

    FRHIBuffer StagingBuffer = GfxDevice.CreateBuffer(StagingBufferDesc);
    if (!StagingBuffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create staging buffer: {}", DebugName);
        return false;
    }

    // 映射 staging buffer 并复制数据
    void* MappedData = GfxDevice.MapBuffer(StagingBuffer, 0, DataSize);
    if (!MappedData)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to map staging buffer: {}", DebugName);
        GfxDevice.DestroyBuffer(StagingBuffer);
        return false;
    }

    memcpy(MappedData, Data, DataSize);
    GfxDevice.UnmapBuffer(StagingBuffer);

    // 创建目标缓冲区（设备本地）
    FRHIBufferDesc DstBufferDesc;
    DstBufferDesc.Size           = DataSize;
    DstBufferDesc.Usage          = Usage | ERHIBufferUsage::TransferDst;
    DstBufferDesc.MemoryProperty = ERHIBufferMemoryProperty::DeviceLocal;
    DstBufferDesc.DebugName      = DebugName;

    DstBuffer = GfxDevice.CreateBuffer(DstBufferDesc);
    if (!DstBuffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create destination buffer: {}", DebugName);
        GfxDevice.DestroyBuffer(StagingBuffer);
        return false;
    }

    // 获取共享的 CommandPool
    FGlobalAssetImporter& GlobalImporter = FGlobalAssetImporter::GetRef();
    FRHICommandPool       CommandPool    = GlobalImporter.GetUploadCommandPool();
    if (!CommandPool.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Global upload command pool is not available");
        GfxDevice.DestroyBuffer(DstBuffer);
        GfxDevice.DestroyBuffer(StagingBuffer);
        return false;
    }

    // 创建命令缓冲区
    FRHICommandBufferDesc CmdBufferDesc;
    CmdBufferDesc.Level      = ERHICommandBufferLevel::Primary;
    CmdBufferDesc.UsageFlags = ERHICommandBufferUsageFlag::OneTimeSubmit;
    CmdBufferDesc.DebugName  = DebugName + "_UploadCmd";

    FRHICommandBuffer CommandBuffer = GfxDevice.CreateCommandBuffer(CommandPool, CmdBufferDesc);
    if (!CommandBuffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create command buffer: {}", DebugName);
        GfxDevice.DestroyBuffer(DstBuffer);
        GfxDevice.DestroyBuffer(StagingBuffer);
        return false;
    }

    // 开始记录命令
    CommandBuffer.Begin(ERHICommandBufferUsageFlag::OneTimeSubmit);

    // 复制数据从 staging buffer 到目标 buffer
    TArray<FRHIBufferCopyRegion> CopyRegions;
    FRHIBufferCopyRegion         CopyRegion;
    CopyRegion.SrcOffset = 0;
    CopyRegion.DstOffset = 0;
    CopyRegion.Size      = DataSize;
    CopyRegions.Add(CopyRegion);

    CommandBuffer.CopyBuffer(StagingBuffer, DstBuffer, CopyRegions);

    // 结束记录命令
    CommandBuffer.End();

    // 执行命令
    CommandBuffer.Execute();

    // 清理资源
    GfxDevice.DestroyCommandBuffer(CommandPool, CommandBuffer);
    GfxDevice.DestroyBuffer(StagingBuffer);

    return true;
}

// 创建并上传顶点缓冲区
bool CreateVertexBuffer(const FMeshData& MeshData, FRHIBuffer& OutVertexBuffer)
{
    const UInt64 VertexBufferSize = static_cast<UInt64>(MeshData.VertexCount) * sizeof(FVertexPNU);
    return UploadBufferDataToGPU(MeshData.Vertices.Data(), VertexBufferSize, OutVertexBuffer,
                                 ERHIBufferUsage::VertexBuffer, "MeshVertexBuffer");
}

// 创建并上传索引缓冲区
bool CreateIndexBuffer(const FMeshData& MeshData, FRHIBuffer& OutIndexBuffer)
{
    const UInt64 IndexBufferSize = static_cast<UInt64>(MeshData.IndexCount) * sizeof(UInt32);
    return UploadBufferDataToGPU(MeshData.Indices.Data(), IndexBufferSize, OutIndexBuffer,
                                 ERHIBufferUsage::IndexBuffer, "MeshIndexBuffer");
}

// 处理单个 SubMesh
bool ProcessSubMesh(const FMeshData& MeshData, FSubMesh& OutSubMesh)
{
    // 创建顶点缓冲区
    if (!CreateVertexBuffer(MeshData, OutSubMesh.VertexBuffer))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create vertex buffer for sub-mesh");
        return false;
    }

    // 创建索引缓冲区
    if (!CreateIndexBuffer(MeshData, OutSubMesh.IndexBuffer))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create index buffer for sub-mesh");
        GetGfxDeviceRef().DestroyBuffer(OutSubMesh.VertexBuffer);
        return false;
    }

    OutSubMesh.VertexCount = MeshData.VertexCount;
    OutSubMesh.IndexCount  = MeshData.IndexCount;

    return true;
}
} // namespace

TSharedPtr<FAssetImportSetting> FMeshImporter::GetOrCreateImportSetting(FAssetMetaData& Metadata)
{
    // 如果已存在导入设置
    if (Metadata.ImportSetting)
    {
        // 尝试转换为 FMeshImportSetting
        if (auto MeshSetting = DynamicPointerCast<FMeshImportSetting>(Metadata.ImportSetting))
        {
            // 类型匹配，返回现有设置
            return MeshSetting;
        }
        else
        {
            // 类型不匹配，这是严重错误
            HK_LOG_FATAL(ELogcat::Asset,
                         "Import setting exists but is not FMeshImportSetting! This should never happen. Path: {}",
                         Metadata.Path);
            return nullptr;
        }
    }

    // 不存在，创建新的设置
    auto NewSetting = MakeShared<FMeshImportSetting>();
    Metadata.ImportSetting = NewSetting;
    return NewSetting;
}

bool FMeshImporter::Import(FStringView Path, EAssetFileType FileType, EAssetImportOptions Options)
{
    // 转换路径为 FString
    FString FilePath(Path);

    // 加载 .meta 文件获取导入设置
    FAssetRegistry& AssetRegistry = FAssetRegistry::GetRef();
    FAssetMetaData& Metadata      = AssetRegistry.LoadAssetMetadata(FilePath);

    // 获取或创建导入设置
    auto MeshSetting = DynamicPointerCast<FMeshImportSetting>(GetOrCreateImportSetting(Metadata));
    if (!MeshSetting)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to get or create import setting for mesh: {}", FilePath);
        return false;
    }

    // 使用导入设置中的标志加载模型数据
    TArray<FMeshData> MeshDataArray;
    if (!LoadMeshData(FilePath, MeshSetting->ImportFlags, MeshDataArray))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to load mesh data: {}", FilePath);
        return false;
    }

    // 创建 HMesh 对象
    FObjectArray& ObjectArray = FObjectArray::GetRef();
    auto*         Mesh       = ObjectArray.CreateObject<HMesh>(FName(FilePath));
    if (!Mesh)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create HMesh object");
        return false;
    }

    // 处理所有 SubMesh
    TArray<FSubMesh>& SubMeshes = Mesh->internalGetMutableSubMeshes();
    SubMeshes.Reserve(MeshDataArray.Size());

    bool bAllSuccess = true;
    for (UInt32 I = 0; I < MeshDataArray.Size(); ++I)
    {
        FSubMesh SubMesh;
        if (ProcessSubMesh(MeshDataArray[I], SubMesh))
        {
            SubMeshes.Add(SubMesh);
            HK_LOG_INFO(ELogcat::Asset, "Processed sub-mesh {}: {} vertices, {} indices", I, SubMesh.VertexCount,
                        SubMesh.IndexCount);
        }
        else
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to process sub-mesh {}", I);
            bAllSuccess = false;
            // 清理已创建的 SubMesh
            for (FSubMesh& CreatedSubMesh : SubMeshes)
            {
                if (CreatedSubMesh.IndexBuffer.IsValid())
                {
                    GetGfxDeviceRef().DestroyBuffer(CreatedSubMesh.IndexBuffer);
                }
                if (CreatedSubMesh.VertexBuffer.IsValid())
                {
                    GetGfxDeviceRef().DestroyBuffer(CreatedSubMesh.VertexBuffer);
                }
            }
            SubMeshes.Clear();
            break;
        }
    }

    if (!bAllSuccess)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to import mesh: {}", FilePath);
        return false;
    }

    // 保存元数据（包含导入设置）
    Metadata.AssetType = EAssetType::Mesh;
    AssetRegistry.SaveAssetMetadata(Metadata);

    HK_LOG_INFO(ELogcat::Asset, "Successfully imported mesh: {} ({} sub-meshes)", FilePath, SubMeshes.Size());
    return true;
}