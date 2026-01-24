#include "Component.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Object/Component.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_CComponent_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<CComponent>("Component");

    // 注册父类: HObject
    Type->RegisterParent(FTypeManager::TypeOf<HObject>());

    // 注册属性
    CComponent::Register_CComponent_Properties(Type);

}

void CComponent::Z_Component_Register::Register_CComponent()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<CComponent>(Register_CComponent_Impl);
}

#define CComponent_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        Ar( \
        MakeNamedPair("bActive", bActive), \
        MakeNamedPair("Owner", Owner) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(CComponent)

#undef CComponent_SERIALIZATION_CODE

