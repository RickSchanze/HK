//
// Created by Admin on 2026/1/4.
//

#include "MeshUtility.h"
#include "Core/Logging/Logger.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHICommandPool.h"
#include "Render/Mesh/Mesh.h"
#include "Render/Mesh/MeshImporter.h"
#include "Render/RenderContext.h"
#include <cstring>

bool FMeshUtility::CreateAndUploadMeshFromIntermediate(const FMeshIntermediate& Intermediate,
                                                        TArray<FSubMesh>& OutSubMeshes,
                                                        TArray<FRHIBuffer>& OutStagingBuffers,
                                                        TArray<FRHICommandBuffer>& OutCommandBuffers)
{
    FGfxDevice&     GfxDevice     = GetGfxDeviceRef();
    FRenderContext& RenderContext  = FRenderContext::GetRef();
    FRHICommandPool CommandPool   = RenderContext.GetUploadCommandPool();
    if (!CommandPool.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Global upload command pool is not available");
        return false;
    }

    OutSubMeshes.Reserve(Intermediate.SubMeshes.Size());

    bool bAllSuccess = true;
    for (UInt32 I = 0; I < Intermediate.SubMeshes.Size(); ++I)
    {
        const FSubMeshIntermediate& SubMeshIntermediate = Intermediate.SubMeshes[I];
        FSubMesh                     SubMesh;

        // 创建顶点缓冲区
        const UInt64 VertexBufferSize =
            static_cast<UInt64>(SubMeshIntermediate.Vertices.Size()) * sizeof(FVertexPNU);

        FRHIBufferDesc StagingVertexBufferDesc;
        StagingVertexBufferDesc.Size           = VertexBufferSize;
        StagingVertexBufferDesc.Usage          = ERHIBufferUsage::TransferSrc;
        StagingVertexBufferDesc.MemoryProperty  = ERHIBufferMemoryProperty::HostVisible | ERHIBufferMemoryProperty::HostCoherent;
        StagingVertexBufferDesc.DebugName      = "MeshVertexStagingBuffer";

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

        memcpy(MappedVertexData, SubMeshIntermediate.Vertices.Data(), VertexBufferSize);
        GfxDevice.UnmapBuffer(StagingVertexBuffer);

        FRHIBufferDesc VertexBufferDesc;
        VertexBufferDesc.Size           = VertexBufferSize;
        VertexBufferDesc.Usage          = ERHIBufferUsage::VertexBuffer | ERHIBufferUsage::TransferDst;
        VertexBufferDesc.MemoryProperty  = ERHIBufferMemoryProperty::DeviceLocal;
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
        const UInt64 IndexBufferSize = static_cast<UInt64>(SubMeshIntermediate.Indices.Size()) * sizeof(UInt32);

        FRHIBufferDesc StagingIndexBufferDesc;
        StagingIndexBufferDesc.Size           = IndexBufferSize;
        StagingIndexBufferDesc.Usage          = ERHIBufferUsage::TransferSrc;
        StagingIndexBufferDesc.MemoryProperty  = ERHIBufferMemoryProperty::HostVisible | ERHIBufferMemoryProperty::HostCoherent;
        StagingIndexBufferDesc.DebugName      = "MeshIndexStagingBuffer";

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

        memcpy(MappedIndexData, SubMeshIntermediate.Indices.Data(), IndexBufferSize);
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
        OutStagingBuffers.Add(StagingVertexBuffer);
        OutStagingBuffers.Add(StagingIndexBuffer);
        OutCommandBuffers.Add(CommandBuffer);

        SubMesh.VertexCount = static_cast<UInt32>(SubMeshIntermediate.Vertices.Size());
        SubMesh.IndexCount  = static_cast<UInt32>(SubMeshIntermediate.Indices.Size());

        OutSubMeshes.Add(SubMesh);

        HK_LOG_INFO(ELogcat::Asset, "Processed sub-mesh {}: {} vertices, {} indices", I, SubMesh.VertexCount,
                    SubMesh.IndexCount);
    }

    if (!bAllSuccess)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to process all sub-meshes from intermediate data");
        return false;
    }

    return true;
}

