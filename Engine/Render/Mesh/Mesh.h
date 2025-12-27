#pragma once
#include "Core/Reflection/Reflection.h"
#include "Object/Asset.h"
#include "RHI/RHIBuffer.h"

#include "Mesh.generated.h"

struct FSubMesh
{
    FRHIBuffer IndexBuffer;
    FRHIBuffer VertexBuffer;
    UInt32     IndexCount;
    UInt32     VertexCount;
};

HCLASS()
class HMesh : public HAsset
{
    GENERATED_BODY(HMesh)
public:
    HMesh();
    ~HMesh() override;

    TArray<FSubMesh>& internalGetMutableSubMeshes()
    {
        return SubMeshes;
    }

    const TArray<FSubMesh>& GetSubMeshes() const
    {
        return SubMeshes;
    }

    // 获取 SubMesh 数量
    UInt32 GetSubMeshCount() const
    {
        return static_cast<UInt32>(SubMeshes.Size());
    }

    // 获取总顶点数（所有 SubMesh 的汇总）
    UInt32 GetTotalVertexCount() const
    {
        UInt32 Total = 0;
        for (const FSubMesh& SubMesh : SubMeshes)
        {
            Total += SubMesh.VertexCount;
        }
        return Total;
    }

    // 获取总索引数（所有 SubMesh 的汇总）
    UInt32 GetTotalIndexCount() const
    {
        UInt32 Total = 0;
        for (const FSubMesh& SubMesh : SubMeshes)
        {
            Total += SubMesh.IndexCount;
        }
        return Total;
    }

    // 获取指定 SubMesh 的顶点数
    UInt32 GetSubMeshVertexCount(UInt32 SubMeshIndex) const
    {
        if (SubMeshIndex >= SubMeshes.Size())
        {
            return 0;
        }
        return SubMeshes[SubMeshIndex].VertexCount;
    }

    // 获取指定 SubMesh 的索引数
    UInt32 GetSubMeshIndexCount(UInt32 SubMeshIndex) const
    {
        if (SubMeshIndex >= SubMeshes.Size())
        {
            return 0;
        }
        return SubMeshes[SubMeshIndex].IndexCount;
    }

private:
    TArray<FSubMesh> SubMeshes;
};
