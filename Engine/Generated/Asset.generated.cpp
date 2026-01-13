#include "Asset.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Object/Asset.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_HAsset_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<HAsset>("HAsset");

    // 注册父类: HObject
    Type->RegisterParent(FTypeManager::TypeOf<HObject>());

    // 注册属性
    HAsset::Register_HAsset_Properties(Type);

}

HK_API void HAsset::Z_HAsset_Register::Register_HAsset()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<HAsset>(Register_HAsset_Impl);
}

#define HAsset_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        Ar( \
        MakeNamedPair("AssetType", AssetType) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(HAsset)

#undef HAsset_SERIALIZATION_CODE


static void Z_Register_EAssetType_Impl()
{
    // 注册枚举类型
    FTypeMutable Type = FTypeManager::Register<EAssetType>("EAssetType");

    // 注册枚举成员: Shader
    Type->RegisterEnumMember(EAssetType::Shader, "Shader");

    // 注册枚举成员: Mesh
    Type->RegisterEnumMember(EAssetType::Mesh, "Mesh");

    // 注册枚举成员: Texture
    Type->RegisterEnumMember(EAssetType::Texture, "Texture");

    // 注册枚举成员: Material
    Type->RegisterEnumMember(EAssetType::Material, "Material");

    // 注册枚举成员: Count
    Type->RegisterEnumMember(EAssetType::Count, "Count");

}

#pragma warning(disable: 4100)  // 禁用未使用参数警告

void Z_Register_EAssetType()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<EAssetType>(Z_Register_EAssetType_Impl);
}
