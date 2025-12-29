#pragma once
#include "Core/Container/Array.h"
#include "Math/Vector.h"
#include "Mesh.h"
#include "Object/AssetImporter.h"

#include "MeshImporter.generated.h"

class FRHICommandBuffer;
// 顶点数据结构（与 Common.slang 中的 Vertex_PNU 对应）
HSTRUCT()
struct FVertexPNU
{
    GENERATED_BODY(FVertexPNU)
public:
    HPROPERTY()
    FVector3f Position;

    HPROPERTY()
    FVector3f Normal;

    HPROPERTY()
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

// 子网格中间数据结构
HSTRUCT()
struct FSubMeshIntermediate
{
    GENERATED_BODY(FSubMeshIntermediate)
public:
    HPROPERTY()
    TArray<FVertexPNU> Vertices;

    HPROPERTY()
    TArray<UInt32> Indices;
};

// 网格中间数据结构
HSTRUCT()
struct FMeshIntermediate
{
    GENERATED_BODY(FMeshIntermediate)
public:
    HPROPERTY()
    UInt64 Hash = 0; // Hash 值（计算时排除此字段）

    HPROPERTY()
    TArray<FSubMeshIntermediate> SubMeshes;
};

HENUM()
enum class EMeshImportFlag : UInt32
{
    None                     = 0,
    Triangulate              = 1 << 0,  // aiProcess_Triangulate
    GenNormals               = 1 << 1,  // aiProcess_GenNormals
    GenSmoothNormals         = 1 << 2,  // aiProcess_GenSmoothNormals
    SplitLargeMeshes         = 1 << 3,  // aiProcess_SplitLargeMeshes
    PreTransformVertices     = 1 << 4,  // aiProcess_PreTransformVertices
    CalcTangentSpace         = 1 << 5,  // aiProcess_CalcTangentSpace
    JoinIdenticalVertices    = 1 << 6,  // aiProcess_JoinIdenticalVertices
    RemoveRedundantMaterials = 1 << 7,  // aiProcess_RemoveRedundantMaterials
    SplitByBoneCount         = 1 << 8,  // aiProcess_SplitByBoneCount
    FindDegenerates          = 1 << 9,  // aiProcess_FindDegenerates
    FindInvalidData          = 1 << 10, // aiProcess_FindInvalidData
    GenUVCoords              = 1 << 11, // aiProcess_GenUVCoords
    TransformUVCoords        = 1 << 12, // aiProcess_TransformUVCoords
    FindInstances            = 1 << 13, // aiProcess_FindInstances
    OptimizeMeshes           = 1 << 14, // aiProcess_OptimizeMeshes
    OptimizeGraph            = 1 << 15, // aiProcess_OptimizeGraph
    FlipUVs                  = 1 << 16, // aiProcess_FlipUVs
    FlipWindingOrder         = 1 << 17, // aiProcess_FlipWindingOrder
    SortByPType              = 1 << 18, // aiProcess_SortByPType
};
HK_ENABLE_BITMASK_OPERATORS(EMeshImportFlag)

HCLASS()
class FMeshImportSetting : public FAssetImportSetting
{
    GENERATED_BODY(FMeshImportSetting)
public:
    HPROPERTY()
    EMeshImportFlag ImportFlags = EMeshImportFlag::Triangulate | EMeshImportFlag::GenNormals |
                                  EMeshImportFlag::FlipUVs | EMeshImportFlag::CalcTangentSpace |
                                  EMeshImportFlag::JoinIdenticalVertices;
};

class FMeshImporter : public FAssetImporter
{
public:
    // 重写基类方法
    void BeginImport() override;
    bool ProcessImport() override;
    bool ProcessAssetIntermediate() override;
    void EndImport() override;

private:
    // 导入过程中的临时数据
    struct FImportData
    {
        TArray<FMeshData> MeshDataArray;
        TArray<FRHIBuffer> StagingBuffers;      // 所有 staging buffer
        TArray<FRHICommandBuffer> CommandBuffers; // 所有 command buffer
        HMesh* Mesh = nullptr;
        EMeshImportFlag ImportFlags = EMeshImportFlag::None;
    };

    FImportData* ImportData = nullptr;
};
