#include "GfxDevice.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "RHI/GfxDevice.h"


static void Z_Register_EGfxBackend_Impl()
{
    // 注册枚举类型
    FTypeMutable Type = FTypeManager::Register<EGfxBackend>("EGfxBackend");

    // 注册枚举成员: Vulkan
    Type->RegisterEnumMember(EGfxBackend::Vulkan, "Vulkan");

    // 注册枚举成员: D3D12
    Type->RegisterEnumMember(EGfxBackend::D3D12, "D3D12");

    // 注册枚举成员: Metal
    Type->RegisterEnumMember(EGfxBackend::Metal, "Metal");

    // 注册枚举成员: GL
    Type->RegisterEnumMember(EGfxBackend::GL, "GL");

    // 注册枚举成员: GLES
    Type->RegisterEnumMember(EGfxBackend::GLES, "GLES");

    // 注册枚举成员: Count
    Type->RegisterEnumMember(EGfxBackend::Count, "Count");

}

#pragma warning(disable: 4100)  // 禁用未使用参数警告

void Z_Register_EGfxBackend()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<EGfxBackend>(Z_Register_EGfxBackend_Impl);
}
