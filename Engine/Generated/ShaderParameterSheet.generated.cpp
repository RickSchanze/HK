#include "ShaderParameterSheet.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Shader/ShaderParameterSheet.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FShaderPushConstantItem_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FShaderPushConstantItem>("ShaderPushConstantItem");

    // 注册属性
    FShaderPushConstantItem::Register_FShaderPushConstantItem_Properties(Type);

}

void FShaderPushConstantItem::Z_ShaderPushConstantItem_Register::Register_FShaderPushConstantItem()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FShaderPushConstantItem>(Register_FShaderPushConstantItem_Impl);
}


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FShaderParameterSheet_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FShaderParameterSheet>("ShaderParameterSheet");

    // 注册属性
    FShaderParameterSheet::Register_FShaderParameterSheet_Properties(Type);

}

void FShaderParameterSheet::Z_ShaderParameterSheet_Register::Register_FShaderParameterSheet()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FShaderParameterSheet>(Register_FShaderParameterSheet_Impl);
}


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FShaderBinaryData_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FShaderBinaryData>("ShaderBinaryData");

    // 注册属性
    FShaderBinaryData::Register_FShaderBinaryData_Properties(Type);

}

void FShaderBinaryData::Z_ShaderBinaryData_Register::Register_FShaderBinaryData()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FShaderBinaryData>(Register_FShaderBinaryData_Impl);
}

