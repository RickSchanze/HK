//
// Created by Admin on 2025/12/27.
//

#include "Mesh.h"

#include "RHI/GfxDevice.h"

HMesh::HMesh()
{
    AssetType = EAssetType::Mesh;
}

HMesh::~HMesh()
{
    FGfxDevice& GfxDevice = GetGfxDeviceRef();

    // 清理所有 SubMesh 的缓冲区
    for (FSubMesh& SubMesh : SubMeshes)
    {
        if (SubMesh.IndexBuffer.IsValid())
        {
            GfxDevice.DestroyBuffer(SubMesh.IndexBuffer);
        }
        if (SubMesh.VertexBuffer.IsValid())
        {
            GfxDevice.DestroyBuffer(SubMesh.VertexBuffer);
        }
    }
}