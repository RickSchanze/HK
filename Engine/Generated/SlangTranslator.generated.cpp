#include "SlangTranslator.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Shader/SlangTranslator.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FShaderTranslateResult_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FShaderTranslateResult>("ShaderTranslateResult");

    // 注册属性
    FShaderTranslateResult::Register_FShaderTranslateResult_Properties(Type);

}

void FShaderTranslateResult::Z_ShaderTranslateResult_Register::Register_FShaderTranslateResult()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FShaderTranslateResult>(Register_FShaderTranslateResult_Impl);
}

