#include "Color.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Math/Color.h"


static void Register_FColor_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FColor>("Color");

    // 注册属性
    FColor::Register_FColor_Properties(Type);

}

HK_API void FColor::Z_Color_Register::Register_FColor()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FColor>(Register_FColor_Impl);
}

