#include "RHISampler.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "RHI/RHISampler.h"


static void Z_Register_ERHIFilter_Impl()
{
    // 注册枚举类型
    FTypeMutable Type = FTypeManager::Register<ERHIFilter>("ERHIFilter");

    // 注册枚举成员: Nearest
    Type->RegisterEnumMember(ERHIFilter::Nearest, "Nearest");

    // 注册枚举成员: Linear
    Type->RegisterEnumMember(ERHIFilter::Linear, "Linear");

}

#pragma warning(disable: 4100)  // 禁用未使用参数警告

void Z_Register_ERHIFilter()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<ERHIFilter>(Z_Register_ERHIFilter_Impl);
}
