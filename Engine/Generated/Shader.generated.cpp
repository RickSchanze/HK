#include "Shader.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Shader/Shader.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_HShader_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<HShader>("HShader");

    // 注册父类: HAsset
    Type->RegisterParent(FTypeManager::TypeOf<HAsset>());

}

void HShader::Z_HShader_Register::Register_HShader()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<HShader>(Register_HShader_Impl);
}

#define HShader_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(HShader)

#undef HShader_SERIALIZATION_CODE

