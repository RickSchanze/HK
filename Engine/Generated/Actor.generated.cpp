#include "Actor.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Object/Actor.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_AActor_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<AActor>("AActor");

    // 注册父类: HObject
    Type->RegisterParent(FTypeManager::TypeOf<HObject>());

    // 注册属性
    AActor::Register_AActor_Properties(Type);

}

void AActor::Z_AActor_Register::Register_AActor()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<AActor>(Register_AActor_Impl);
}

#define AActor_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        Ar( \
        MakeNamedPair("Components", Components), \
        MakeNamedPair("RelativeTransform", RelativeTransform) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(AActor)

#undef AActor_SERIALIZATION_CODE

