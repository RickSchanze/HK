#include "RHIConfig.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "RHI/RHIConfig.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FRHIConfig_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FRHIConfig>("RHIConfig");

    // 注册父类: IConfig
    Type->RegisterParent(FTypeManager::TypeOf<IConfig>());

    // 注册属性
    FRHIConfig::Register_FRHIConfig_Properties(Type);

    // 注册类型属性: ConfigPath = Config/RHIConfig.xml
    Type->RegisterAttribute(FName("ConfigPath"), "Config/RHIConfig.xml");

}

void FRHIConfig::Z_RHIConfig_Register::Register_FRHIConfig()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FRHIConfig>(Register_FRHIConfig_Impl);
}

#define FRHIConfig_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        Ar( \
        MakeNamedPair("DefaultWindowSize", DefaultWindowSize), \
        MakeNamedPair("GfxBackend", GfxBackend) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(FRHIConfig)

#undef FRHIConfig_SERIALIZATION_CODE

