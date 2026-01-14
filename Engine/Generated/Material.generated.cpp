#include "Material.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Material/Material.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_HMaterial_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<HMaterial>("HMaterial");

    // 注册父类: HAsset
    Type->RegisterParent(FTypeManager::TypeOf<HAsset>());

    // 注册属性
    HMaterial::Register_HMaterial_Properties(Type);

}

void HMaterial::Z_HMaterial_Register::Register_HMaterial()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<HMaterial>(Register_HMaterial_Impl);
}

#define HMaterial_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        Ar( \
        MakeNamedPair("Shader", Shader) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(HMaterial)

#undef HMaterial_SERIALIZATION_CODE

