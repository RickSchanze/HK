#include "EngineConfig.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Config/EngineConfig.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FEngineConfig_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FEngineConfig>("EngineConfig");

    // 注册父类: IConfig
    Type->RegisterParent(FTypeManager::TypeOf<IConfig>());

    // 注册属性
    FEngineConfig::Register_FEngineConfig_Properties(Type);

    // 注册类型属性: ConfigPath = Config/Engine.xml
    Type->RegisterAttribute(FName("ConfigPath"), "Config/Engine.xml");

}

HK_API void FEngineConfig::Z_EngineConfig_Register::Register_FEngineConfig()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FEngineConfig>(Register_FEngineConfig_Impl);
}

#define FEngineConfig_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        Ar( \
        MakeNamedPair("DefaultObjectCount", DefaultObjectCount), \
        MakeNamedPair("DefaultObjectIncreaseCount", DefaultObjectIncreaseCount) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(FEngineConfig)

#undef FEngineConfig_SERIALIZATION_CODE

