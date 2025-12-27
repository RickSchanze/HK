#pragma once
#include "Object/AssetImporter.h"

#include "MeshImporter.generated.h"

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
    bool Import(FStringView Path, EAssetFileType FileType, EAssetImportOptions Options) override;

    TSharedPtr<FAssetImportSetting> GetOrCreateImportSetting(FAssetMetaData& Metadata) override;
};
