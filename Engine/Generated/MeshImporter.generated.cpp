#include "MeshImporter.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Mesh/MeshImporter.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FMeshImportSetting_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FMeshImportSetting>("MeshImportSetting");

    // 注册父类: FAssetImportSetting
    Type->RegisterParent(FTypeManager::TypeOf<FAssetImportSetting>());

    // 注册属性
    FMeshImportSetting::Register_FMeshImportSetting_Properties(Type);

}

void FMeshImportSetting::Z_MeshImportSetting_Register::Register_FMeshImportSetting()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FMeshImportSetting>(Register_FMeshImportSetting_Impl);
}

#define FMeshImportSetting_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        Ar( \
        MakeNamedPair("ImportFlags", ImportFlags) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(FMeshImportSetting)

#undef FMeshImportSetting_SERIALIZATION_CODE


static void Z_Register_EMeshImportFlag_Impl()
{
    // 注册枚举类型
    FTypeMutable Type = FTypeManager::Register<EMeshImportFlag>("EMeshImportFlag");

    // 注册枚举成员: None
    Type->RegisterEnumMember(EMeshImportFlag::None, "None");

    // 注册枚举成员: Triangulate
    Type->RegisterEnumMember(EMeshImportFlag::Triangulate, "Triangulate");

    // 注册枚举成员: GenNormals
    Type->RegisterEnumMember(EMeshImportFlag::GenNormals, "GenNormals");

    // 注册枚举成员: GenSmoothNormals
    Type->RegisterEnumMember(EMeshImportFlag::GenSmoothNormals, "GenSmoothNormals");

    // 注册枚举成员: SplitLargeMeshes
    Type->RegisterEnumMember(EMeshImportFlag::SplitLargeMeshes, "SplitLargeMeshes");

    // 注册枚举成员: PreTransformVertices
    Type->RegisterEnumMember(EMeshImportFlag::PreTransformVertices, "PreTransformVertices");

    // 注册枚举成员: CalcTangentSpace
    Type->RegisterEnumMember(EMeshImportFlag::CalcTangentSpace, "CalcTangentSpace");

    // 注册枚举成员: JoinIdenticalVertices
    Type->RegisterEnumMember(EMeshImportFlag::JoinIdenticalVertices, "JoinIdenticalVertices");

    // 注册枚举成员: RemoveRedundantMaterials
    Type->RegisterEnumMember(EMeshImportFlag::RemoveRedundantMaterials, "RemoveRedundantMaterials");

    // 注册枚举成员: SplitByBoneCount
    Type->RegisterEnumMember(EMeshImportFlag::SplitByBoneCount, "SplitByBoneCount");

    // 注册枚举成员: FindDegenerates
    Type->RegisterEnumMember(EMeshImportFlag::FindDegenerates, "FindDegenerates");

    // 注册枚举成员: FindInvalidData
    Type->RegisterEnumMember(EMeshImportFlag::FindInvalidData, "FindInvalidData");

    // 注册枚举成员: GenUVCoords
    Type->RegisterEnumMember(EMeshImportFlag::GenUVCoords, "GenUVCoords");

    // 注册枚举成员: TransformUVCoords
    Type->RegisterEnumMember(EMeshImportFlag::TransformUVCoords, "TransformUVCoords");

    // 注册枚举成员: FindInstances
    Type->RegisterEnumMember(EMeshImportFlag::FindInstances, "FindInstances");

    // 注册枚举成员: OptimizeMeshes
    Type->RegisterEnumMember(EMeshImportFlag::OptimizeMeshes, "OptimizeMeshes");

    // 注册枚举成员: OptimizeGraph
    Type->RegisterEnumMember(EMeshImportFlag::OptimizeGraph, "OptimizeGraph");

    // 注册枚举成员: FlipUVs
    Type->RegisterEnumMember(EMeshImportFlag::FlipUVs, "FlipUVs");

    // 注册枚举成员: FlipWindingOrder
    Type->RegisterEnumMember(EMeshImportFlag::FlipWindingOrder, "FlipWindingOrder");

    // 注册枚举成员: SortByPType
    Type->RegisterEnumMember(EMeshImportFlag::SortByPType, "SortByPType");

}

#pragma warning(disable: 4100)  // 禁用未使用参数警告

void Z_Register_EMeshImportFlag()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<EMeshImportFlag>(Z_Register_EMeshImportFlag_Impl);
}
