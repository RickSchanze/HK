#include "ShaderImporter.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Shader/ShaderImporter.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FShaderImportSetting_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FShaderImportSetting>("ShaderImportSetting");

    // 注册父类: FAssetImportSetting
    Type->RegisterParent(FTypeManager::TypeOf<FAssetImportSetting>());

}

void FShaderImportSetting::Z_ShaderImportSetting_Register::Register_FShaderImportSetting()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FShaderImportSetting>(Register_FShaderImportSetting_Impl);
}

#define FShaderImportSetting_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(FShaderImportSetting)

#undef FShaderImportSetting_SERIALIZATION_CODE

