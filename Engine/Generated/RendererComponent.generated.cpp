#include "RendererComponent.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Components/RendererComponent.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_CRendererComponent_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<CRendererComponent>("RendererComponent");

    // 注册父类: CSceneComponent
    Type->RegisterParent(FTypeManager::TypeOf<CSceneComponent>());

}

void CRendererComponent::Z_RendererComponent_Register::Register_CRendererComponent()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<CRendererComponent>(Register_CRendererComponent_Impl);
}

#define CRendererComponent_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(CRendererComponent)

#undef CRendererComponent_SERIALIZATION_CODE

