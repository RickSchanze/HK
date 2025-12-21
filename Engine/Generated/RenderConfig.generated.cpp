#include "RenderConfig.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/RenderConfig.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FRenderConfig_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FRenderConfig>("RenderConfig");

    // 注册父类: IConfig
    Type->RegisterParent(FTypeManager::TypeOf<IConfig>());

    // 注册类型属性: ConfigPath = Config/RenderConfig.xml
    Type->RegisterAttribute(FName("ConfigPath"), "Config/RenderConfig.xml");

}

void FRenderConfig::Z_RenderConfig_Register::Register_FRenderConfig()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FRenderConfig>(Register_FRenderConfig_Impl);
}

#define FRenderConfig_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(FRenderConfig)

#undef FRenderConfig_SERIALIZATION_CODE

