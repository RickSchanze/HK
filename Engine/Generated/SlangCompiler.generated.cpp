#include "SlangCompiler.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Shader/SlangCompiler.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FShaderCompileResult_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FShaderCompileResult>("ShaderCompileResult");

    // 注册属性
    FShaderCompileResult::Register_FShaderCompileResult_Properties(Type);

}

void FShaderCompileResult::Z_ShaderCompileResult_Register::Register_FShaderCompileResult()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FShaderCompileResult>(Register_FShaderCompileResult_Impl);
}

