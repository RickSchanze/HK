//
// Created by Admin on 2025/12/27.
//

#include "MeshImporter.h"

#include "Core/Container/Array.h"
#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/FileUtility.h"
#include "Core/Utility/HashUtility.h"
#include "Core/Utility/Profiler.h"
#include "Core/Utility/SharedPtr.h"
#include "Core/Utility/Uuid.h"
#include "Math/Vector.h"
#include "Object/AssetImporter.h"
#include "Object/AssetRegistry.h"
#include "Object/Object.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHIBuffer.h"
#include "RHI/RHICommandBuffer.h"
#include "RHI/RHICommandPool.h"
#include "Render/Mesh/Mesh.h"
#include "Render/RenderContext.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>
#include <fstream>

namespace
{
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

    const bool bHasNormals   = InMesh->HasNormals();
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

// 获取中间文件路径
FString GetIntermediatePath(const FUuid& Guid)
{
    return FString("Intermediate/Meshes/") + Guid.ToString() + ".bin";
}
} // namespace

void FMeshImporter::BeginImport()
{
    // 分配导入数据
    ImportData = New<FImportData>();
}

bool FMeshImporter::ProcessImport()
{
    if (!Metadata)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Metadata is null in ProcessImport");
        return false;
    }

    // 获取或创建导入设置
    if (!Metadata->ImportSetting)
    {
        Metadata->ImportSetting = MakeShared<FMeshImportSetting>();
    }

    auto MeshSetting = DynamicPointerCast<FMeshImportSetting>(Metadata->ImportSetting);
    if (!MeshSetting)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to get import setting for mesh: {}", Metadata->Path);
        return false;
    }

    // 保存导入标志
    ImportData->ImportFlags = MeshSetting->ImportFlags;

    // 使用导入设置中的标志加载模型数据
    if (!LoadMeshData(Metadata->Path, ImportData->ImportFlags, ImportData->MeshDataArray))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to load mesh data: {}", Metadata->Path);
        return false;
    }

    // 创建 HMesh 对象
    FObjectArray& ObjectArray = FObjectArray::GetRef();
    ImportData->Mesh          = ObjectArray.CreateObject<HMesh>(FName(Metadata->Path));
    if (!ImportData->Mesh)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create HMesh object");
        return false;
    }

    // 处理所有 SubMesh
    TArray<FSubMesh>& SubMeshes = ImportData->Mesh->internalGetMutableSubMeshes();
    SubMeshes.Reserve(ImportData->MeshDataArray.Size());

    FGfxDevice&     GfxDevice     = GetGfxDeviceRef();
    FRenderContext& RenderContext = FRenderContext::GetRef();
    FRHICommandPool CommandPool   = RenderContext.GetUploadCommandPool();
    if (!CommandPool.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Global upload command pool is not available");
        return false;
    }

    bool bAllSuccess = true;
    for (UInt32 I = 0; I < ImportData->MeshDataArray.Size(); ++I)
    {
        const FMeshData& MeshData = ImportData->MeshDataArray[I];
        FSubMesh         SubMesh;

        // 创建顶点缓冲区
        const UInt64 VertexBufferSize = static_cast<UInt64>(MeshData.VertexCount) * sizeof(FVertexPNU);

        FRHIBufferDesc StagingVertexBufferDesc;
        StagingVertexBufferDesc.Size  = VertexBufferSize;
        StagingVertexBufferDesc.Usage = ERHIBufferUsage::TransferSrc;
        StagingVertexBufferDesc.MemoryProperty =
            ERHIBufferMemoryProperty::HostVisible | ERHIBufferMemoryProperty::HostCoherent;
        StagingVertexBufferDesc.DebugName = "MeshVertexStagingBuffer";

        FRHIBuffer StagingVertexBuffer = GfxDevice.CreateBuffer(StagingVertexBufferDesc);
        if (!StagingVertexBuffer.IsValid())
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to create staging vertex buffer for sub-mesh {}", I);
            bAllSuccess = false;
            break;
        }

        void* MappedVertexData = GfxDevice.MapBuffer(StagingVertexBuffer, 0, VertexBufferSize);
        if (!MappedVertexData)
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to map staging vertex buffer for sub-mesh {}", I);
            GfxDevice.DestroyBuffer(StagingVertexBuffer);
            bAllSuccess = false;
            break;
        }

        memcpy(MappedVertexData, MeshData.Vertices.Data(), VertexBufferSize);
        GfxDevice.UnmapBuffer(StagingVertexBuffer);

        FRHIBufferDesc VertexBufferDesc;
        VertexBufferDesc.Size           = VertexBufferSize;
        VertexBufferDesc.Usage          = ERHIBufferUsage::VertexBuffer | ERHIBufferUsage::TransferDst;
        VertexBufferDesc.MemoryProperty = ERHIBufferMemoryProperty::DeviceLocal;
        VertexBufferDesc.DebugName      = "MeshVertexBuffer";

        SubMesh.VertexBuffer = GfxDevice.CreateBuffer(VertexBufferDesc);
        if (!SubMesh.VertexBuffer.IsValid())
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to create vertex buffer for sub-mesh {}", I);
            GfxDevice.DestroyBuffer(StagingVertexBuffer);
            bAllSuccess = false;
            break;
        }

        // 创建索引缓冲区
        const UInt64 IndexBufferSize = static_cast<UInt64>(MeshData.IndexCount) * sizeof(UInt32);

        FRHIBufferDesc StagingIndexBufferDesc;
        StagingIndexBufferDesc.Size  = IndexBufferSize;
        StagingIndexBufferDesc.Usage = ERHIBufferUsage::TransferSrc;
        StagingIndexBufferDesc.MemoryProperty =
            ERHIBufferMemoryProperty::HostVisible | ERHIBufferMemoryProperty::HostCoherent;
        StagingIndexBufferDesc.DebugName = "MeshIndexStagingBuffer";

        FRHIBuffer StagingIndexBuffer = GfxDevice.CreateBuffer(StagingIndexBufferDesc);
        if (!StagingIndexBuffer.IsValid())
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to create staging index buffer for sub-mesh {}", I);
            GfxDevice.DestroyBuffer(SubMesh.VertexBuffer);
            GfxDevice.DestroyBuffer(StagingVertexBuffer);
            bAllSuccess = false;
            break;
        }

        void* MappedIndexData = GfxDevice.MapBuffer(StagingIndexBuffer, 0, IndexBufferSize);
        if (!MappedIndexData)
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to map staging index buffer for sub-mesh {}", I);
            GfxDevice.DestroyBuffer(StagingIndexBuffer);
            GfxDevice.DestroyBuffer(SubMesh.VertexBuffer);
            GfxDevice.DestroyBuffer(StagingVertexBuffer);
            bAllSuccess = false;
            break;
        }

        memcpy(MappedIndexData, MeshData.Indices.Data(), IndexBufferSize);
        GfxDevice.UnmapBuffer(StagingIndexBuffer);

        FRHIBufferDesc IndexBufferDesc;
        IndexBufferDesc.Size           = IndexBufferSize;
        IndexBufferDesc.Usage          = ERHIBufferUsage::IndexBuffer | ERHIBufferUsage::TransferDst;
        IndexBufferDesc.MemoryProperty = ERHIBufferMemoryProperty::DeviceLocal;
        IndexBufferDesc.DebugName      = "MeshIndexBuffer";

        SubMesh.IndexBuffer = GfxDevice.CreateBuffer(IndexBufferDesc);
        if (!SubMesh.IndexBuffer.IsValid())
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to create index buffer for sub-mesh {}", I);
            GfxDevice.DestroyBuffer(StagingIndexBuffer);
            GfxDevice.DestroyBuffer(SubMesh.VertexBuffer);
            GfxDevice.DestroyBuffer(StagingVertexBuffer);
            bAllSuccess = false;
            break;
        }

        // 创建命令缓冲区
        FRHICommandBufferDesc CmdBufferDesc;
        CmdBufferDesc.Level      = ERHICommandBufferLevel::Primary;
        CmdBufferDesc.UsageFlags = ERHICommandBufferUsageFlag::OneTimeSubmit;
        CmdBufferDesc.DebugName  = "MeshUploadCommandBuffer";

        FRHICommandBuffer CommandBuffer = GfxDevice.CreateCommandBuffer(CommandPool, CmdBufferDesc);
        if (!CommandBuffer.IsValid())
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to create command buffer for sub-mesh {}", I);
            GfxDevice.DestroyBuffer(SubMesh.IndexBuffer);
            GfxDevice.DestroyBuffer(StagingIndexBuffer);
            GfxDevice.DestroyBuffer(SubMesh.VertexBuffer);
            GfxDevice.DestroyBuffer(StagingVertexBuffer);
            bAllSuccess = false;
            break;
        }

        // 开始记录命令
        CommandBuffer.Begin(ERHICommandBufferUsageFlag::OneTimeSubmit);

        // 复制顶点数据
        TArray<FRHIBufferCopyRegion> CopyRegions;
        FRHIBufferCopyRegion         CopyRegion;
        CopyRegion.SrcOffset = 0;
        CopyRegion.DstOffset = 0;
        CopyRegion.Size      = VertexBufferSize;
        CopyRegions.Add(CopyRegion);
        CommandBuffer.CopyBuffer(StagingVertexBuffer, SubMesh.VertexBuffer, CopyRegions);

        // 复制索引数据
        CopyRegions.Clear();
        CopyRegion.Size = IndexBufferSize;
        CopyRegions.Add(CopyRegion);
        CommandBuffer.CopyBuffer(StagingIndexBuffer, SubMesh.IndexBuffer, CopyRegions);

        // 结束记录命令
        CommandBuffer.End();

        // 执行命令
        CommandBuffer.Execute();

        // 保存 staging buffer 和 command buffer 以便后续清理
        ImportData->StagingBuffers.Add(StagingVertexBuffer);
        ImportData->StagingBuffers.Add(StagingIndexBuffer);
        ImportData->CommandBuffers.Add(CommandBuffer);

        SubMesh.VertexCount = MeshData.VertexCount;
        SubMesh.IndexCount  = MeshData.IndexCount;

        SubMeshes.Add(SubMesh);

        HK_LOG_INFO(ELogcat::Asset, "Processed sub-mesh {}: {} vertices, {} indices", I, SubMesh.VertexCount,
                    SubMesh.IndexCount);
    }

    if (!bAllSuccess)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to process all sub-meshes");
        return false;
    }

    // 保存元数据
    Metadata->AssetType = EAssetType::Mesh;
    FAssetRegistry::GetRef().SaveAssetMetadata(Metadata);

    HK_LOG_INFO(ELogcat::Asset, "Successfully processed mesh import: {} ({} sub-meshes)", Metadata->Path,
                SubMeshes.Size());
    return true;
}

bool FMeshImporter::ProcessAssetIntermediate()
{
    if (!Metadata || !ImportData)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Metadata or ImportData is null in ProcessAssetIntermediate");
        return false;
    }

    // 获取中间文件路径
    FString IntermediatePath = GetIntermediatePath(Metadata->Uuid);

    // 确保目录存在
    std::filesystem::path Path(IntermediatePath.CStr());
    std::filesystem::path DirPath = Path.parent_path();
    if (!DirPath.empty() && !std::filesystem::exists(DirPath))
    {
        std::error_code ErrorCode;
        if (!std::filesystem::create_directories(DirPath, ErrorCode))
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to create intermediate directory: {}", DirPath.string());
            return false;
        }
    }

    // 准备要写入的数据（不包括 Hash 本身）
    // 格式：SubMeshCount, 然后每个 SubMesh: VertexCount, IndexCount, Vertices, Indices
    struct FIntermediateHeader
    {
        UInt32 SubMeshCount;
    };

    FIntermediateHeader Header;
    Header.SubMeshCount = static_cast<UInt32>(ImportData->MeshDataArray.Size());

    // 计算总数据大小
    size_t TotalSize = sizeof(FIntermediateHeader);
    for (const FMeshData& MeshData : ImportData->MeshDataArray)
    {
        TotalSize += sizeof(UInt32);                                                 // VertexCount
        TotalSize += sizeof(UInt32);                                                 // IndexCount
        TotalSize += static_cast<size_t>(MeshData.VertexCount) * sizeof(FVertexPNU); // Vertices
        TotalSize += static_cast<size_t>(MeshData.IndexCount) * sizeof(UInt32);      // Indices
    }

    // 构建数据缓冲区用于 Hash 计算
    TArray<UInt8> DataBuffer;
    DataBuffer.Reserve(TotalSize);

    // 添加 Header
    const UInt8* HeaderPtr = reinterpret_cast<const UInt8*>(&Header);
    DataBuffer.Append(HeaderPtr, HeaderPtr + sizeof(FIntermediateHeader));

    // 添加每个 SubMesh 的数据
    for (const FMeshData& MeshData : ImportData->MeshDataArray)
    {
        // VertexCount
        const UInt8* VertexCountPtr = reinterpret_cast<const UInt8*>(&MeshData.VertexCount);
        DataBuffer.Append(VertexCountPtr, VertexCountPtr + sizeof(UInt32));

        // IndexCount
        const UInt8* IndexCountPtr = reinterpret_cast<const UInt8*>(&MeshData.IndexCount);
        DataBuffer.Append(IndexCountPtr, IndexCountPtr + sizeof(UInt32));

        // Vertices
        const UInt8* VerticesPtr  = reinterpret_cast<const UInt8*>(MeshData.Vertices.Data());
        const size_t VerticesSize = static_cast<size_t>(MeshData.VertexCount) * sizeof(FVertexPNU);
        DataBuffer.Append(VerticesPtr, VerticesPtr + VerticesSize);

        // Indices
        const UInt8* IndicesPtr  = reinterpret_cast<const UInt8*>(MeshData.Indices.Data());
        const size_t IndicesSize = static_cast<size_t>(MeshData.IndexCount) * sizeof(UInt32);
        DataBuffer.Append(IndicesPtr, IndicesPtr + IndicesSize);
    }

    // 计算 Hash
    const UInt64 Hash = FHashUtility::ComputeHash(DataBuffer.Data(), TotalSize);

    // 保存二进制数据到文件
    std::ofstream File(IntermediatePath.CStr(), std::ios::binary);
    if (!File.is_open())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to open intermediate file for writing: {}", IntermediatePath);
        return false;
    }

    // 先写入 Hash
    File.write(reinterpret_cast<const char*>(&Hash), sizeof(UInt64));

    // 写入 Header
    File.write(reinterpret_cast<const char*>(&Header), sizeof(FIntermediateHeader));

    // 写入每个 SubMesh 的数据
    for (const FMeshData& MeshData : ImportData->MeshDataArray)
    {
        // VertexCount
        File.write(reinterpret_cast<const char*>(&MeshData.VertexCount), sizeof(UInt32));

        // IndexCount
        File.write(reinterpret_cast<const char*>(&MeshData.IndexCount), sizeof(UInt32));

        // Vertices
        const size_t VerticesSize = static_cast<size_t>(MeshData.VertexCount) * sizeof(FVertexPNU);
        File.write(reinterpret_cast<const char*>(MeshData.Vertices.Data()), VerticesSize);

        // Indices
        const size_t IndicesSize = static_cast<size_t>(MeshData.IndexCount) * sizeof(UInt32);
        File.write(reinterpret_cast<const char*>(MeshData.Indices.Data()), IndicesSize);
    }

    if (!File.good())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to write intermediate file: {}", IntermediatePath);
        File.close();
        return false;
    }

    File.close();

    // 更新 Metadata 中的 Hash
    Metadata->IntermediateHash = Hash;
    FAssetRegistry::GetRef().SaveAssetMetadata(Metadata);

    HK_LOG_INFO(ELogcat::Asset, "Saved intermediate mesh data to: {} (Hash: {})", IntermediatePath, Hash);
    return true;
}

void FMeshImporter::EndImport()
{
    if (!ImportData)
    {
        return;
    }

    FGfxDevice&     GfxDevice     = GetGfxDeviceRef();
    FRenderContext& RenderContext = FRenderContext::GetRef();
    FRHICommandPool CommandPool   = RenderContext.GetUploadCommandPool();

    // 清理所有 CommandBuffer
    for (FRHICommandBuffer& CommandBuffer : ImportData->CommandBuffers)
    {
        if (CommandBuffer.IsValid())
        {
            GfxDevice.DestroyCommandBuffer(CommandPool, CommandBuffer);
        }
    }

    // 清理所有 StagingBuffer
    for (FRHIBuffer& StagingBuffer : ImportData->StagingBuffers)
    {
        if (StagingBuffer.IsValid())
        {
            GfxDevice.DestroyBuffer(StagingBuffer);
        }
    }

    // 删除导入数据
    Delete(ImportData);
    ImportData = nullptr;
}

