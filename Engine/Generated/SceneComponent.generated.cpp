#include "SceneComponent.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Object/SceneComponent.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_CSceneComponent_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<CSceneComponent>("SceneComponent");

    // 注册父类: CComponent
    Type->RegisterParent(FTypeManager::TypeOf<CComponent>());

    // 注册属性
    CSceneComponent::Register_CSceneComponent_Properties(Type);

}

void CSceneComponent::Z_SceneComponent_Register::Register_CSceneComponent()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<CSceneComponent>(Register_CSceneComponent_Impl);
}

#define CSceneComponent_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        Ar( \
        MakeNamedPair("LocalTransform", LocalTransform) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(CSceneComponent)

#undef CSceneComponent_SERIALIZATION_CODE

