#include "IConfig.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Config/IConfig.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_IConfig_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<IConfig>("IConfig");

    // 注册类型属性: Interface
    Type->RegisterAttribute(FName("Interface"), FName());

}

HK_API void IConfig::Z_IConfig_Register::Register_IConfig()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<IConfig>(Register_IConfig_Impl);
}

#define IConfig_SERIALIZATION_CODE \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(IConfig)

#undef IConfig_SERIALIZATION_CODE

