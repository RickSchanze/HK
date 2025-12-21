#include "RHIImage.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "RHI/RHIImage.h"


static void Z_Register_ERHIImageFormat_Impl()
{
    // 注册枚举类型
    FTypeMutable Type = FTypeManager::Register<ERHIImageFormat>("ERHIImageFormat");

    // 注册枚举成员: Undefined
    Type->RegisterEnumMember(ERHIImageFormat::Undefined, "Undefined");

    // 注册枚举成员: R8G8B8A8_UNorm
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8A8_UNorm, "R8G8B8A8_UNorm");

    // 注册枚举成员: R8G8B8A8_SNorm
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8A8_SNorm, "R8G8B8A8_SNorm");

    // 注册枚举成员: R8G8B8A8_UScaled
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8A8_UScaled, "R8G8B8A8_UScaled");

    // 注册枚举成员: R8G8B8A8_SScaled
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8A8_SScaled, "R8G8B8A8_SScaled");

    // 注册枚举成员: R8G8B8A8_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8A8_UInt, "R8G8B8A8_UInt");

    // 注册枚举成员: R8G8B8A8_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8A8_SInt, "R8G8B8A8_SInt");

    // 注册枚举成员: R8G8B8A8_SRGB
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8A8_SRGB, "R8G8B8A8_SRGB");

    // 注册枚举成员: B8G8R8A8_UNorm
    Type->RegisterEnumMember(ERHIImageFormat::B8G8R8A8_UNorm, "B8G8R8A8_UNorm");

    // 注册枚举成员: B8G8R8A8_SRGB
    Type->RegisterEnumMember(ERHIImageFormat::B8G8R8A8_SRGB, "B8G8R8A8_SRGB");

    // 注册枚举成员: R8G8B8_UNorm
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8_UNorm, "R8G8B8_UNorm");

    // 注册枚举成员: R8G8B8_SNorm
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8_SNorm, "R8G8B8_SNorm");

    // 注册枚举成员: R8G8B8_UScaled
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8_UScaled, "R8G8B8_UScaled");

    // 注册枚举成员: R8G8B8_SScaled
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8_SScaled, "R8G8B8_SScaled");

    // 注册枚举成员: R8G8B8_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8_UInt, "R8G8B8_UInt");

    // 注册枚举成员: R8G8B8_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8_SInt, "R8G8B8_SInt");

    // 注册枚举成员: R8G8B8_SRGB
    Type->RegisterEnumMember(ERHIImageFormat::R8G8B8_SRGB, "R8G8B8_SRGB");

    // 注册枚举成员: R8_UNorm
    Type->RegisterEnumMember(ERHIImageFormat::R8_UNorm, "R8_UNorm");

    // 注册枚举成员: R8_SNorm
    Type->RegisterEnumMember(ERHIImageFormat::R8_SNorm, "R8_SNorm");

    // 注册枚举成员: R8_UScaled
    Type->RegisterEnumMember(ERHIImageFormat::R8_UScaled, "R8_UScaled");

    // 注册枚举成员: R8_SScaled
    Type->RegisterEnumMember(ERHIImageFormat::R8_SScaled, "R8_SScaled");

    // 注册枚举成员: R8_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R8_UInt, "R8_UInt");

    // 注册枚举成员: R8_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R8_SInt, "R8_SInt");

    // 注册枚举成员: R16_UNorm
    Type->RegisterEnumMember(ERHIImageFormat::R16_UNorm, "R16_UNorm");

    // 注册枚举成员: R16_SNorm
    Type->RegisterEnumMember(ERHIImageFormat::R16_SNorm, "R16_SNorm");

    // 注册枚举成员: R16_UScaled
    Type->RegisterEnumMember(ERHIImageFormat::R16_UScaled, "R16_UScaled");

    // 注册枚举成员: R16_SScaled
    Type->RegisterEnumMember(ERHIImageFormat::R16_SScaled, "R16_SScaled");

    // 注册枚举成员: R16_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R16_UInt, "R16_UInt");

    // 注册枚举成员: R16_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R16_SInt, "R16_SInt");

    // 注册枚举成员: R16_SFloat
    Type->RegisterEnumMember(ERHIImageFormat::R16_SFloat, "R16_SFloat");

    // 注册枚举成员: R16G16_UNorm
    Type->RegisterEnumMember(ERHIImageFormat::R16G16_UNorm, "R16G16_UNorm");

    // 注册枚举成员: R16G16_SNorm
    Type->RegisterEnumMember(ERHIImageFormat::R16G16_SNorm, "R16G16_SNorm");

    // 注册枚举成员: R16G16_UScaled
    Type->RegisterEnumMember(ERHIImageFormat::R16G16_UScaled, "R16G16_UScaled");

    // 注册枚举成员: R16G16_SScaled
    Type->RegisterEnumMember(ERHIImageFormat::R16G16_SScaled, "R16G16_SScaled");

    // 注册枚举成员: R16G16_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R16G16_UInt, "R16G16_UInt");

    // 注册枚举成员: R16G16_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R16G16_SInt, "R16G16_SInt");

    // 注册枚举成员: R16G16_SFloat
    Type->RegisterEnumMember(ERHIImageFormat::R16G16_SFloat, "R16G16_SFloat");

    // 注册枚举成员: R16G16B16_UNorm
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16_UNorm, "R16G16B16_UNorm");

    // 注册枚举成员: R16G16B16_SNorm
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16_SNorm, "R16G16B16_SNorm");

    // 注册枚举成员: R16G16B16_UScaled
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16_UScaled, "R16G16B16_UScaled");

    // 注册枚举成员: R16G16B16_SScaled
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16_SScaled, "R16G16B16_SScaled");

    // 注册枚举成员: R16G16B16_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16_UInt, "R16G16B16_UInt");

    // 注册枚举成员: R16G16B16_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16_SInt, "R16G16B16_SInt");

    // 注册枚举成员: R16G16B16_SFloat
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16_SFloat, "R16G16B16_SFloat");

    // 注册枚举成员: R16G16B16A16_UNorm
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16A16_UNorm, "R16G16B16A16_UNorm");

    // 注册枚举成员: R16G16B16A16_SNorm
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16A16_SNorm, "R16G16B16A16_SNorm");

    // 注册枚举成员: R16G16B16A16_UScaled
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16A16_UScaled, "R16G16B16A16_UScaled");

    // 注册枚举成员: R16G16B16A16_SScaled
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16A16_SScaled, "R16G16B16A16_SScaled");

    // 注册枚举成员: R16G16B16A16_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16A16_UInt, "R16G16B16A16_UInt");

    // 注册枚举成员: R16G16B16A16_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16A16_SInt, "R16G16B16A16_SInt");

    // 注册枚举成员: R16G16B16A16_SFloat
    Type->RegisterEnumMember(ERHIImageFormat::R16G16B16A16_SFloat, "R16G16B16A16_SFloat");

    // 注册枚举成员: R32_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R32_UInt, "R32_UInt");

    // 注册枚举成员: R32_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R32_SInt, "R32_SInt");

    // 注册枚举成员: R32_SFloat
    Type->RegisterEnumMember(ERHIImageFormat::R32_SFloat, "R32_SFloat");

    // 注册枚举成员: R32G32_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R32G32_UInt, "R32G32_UInt");

    // 注册枚举成员: R32G32_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R32G32_SInt, "R32G32_SInt");

    // 注册枚举成员: R32G32_SFloat
    Type->RegisterEnumMember(ERHIImageFormat::R32G32_SFloat, "R32G32_SFloat");

    // 注册枚举成员: R32G32B32_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R32G32B32_UInt, "R32G32B32_UInt");

    // 注册枚举成员: R32G32B32_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R32G32B32_SInt, "R32G32B32_SInt");

    // 注册枚举成员: R32G32B32_SFloat
    Type->RegisterEnumMember(ERHIImageFormat::R32G32B32_SFloat, "R32G32B32_SFloat");

    // 注册枚举成员: R32G32B32A32_UInt
    Type->RegisterEnumMember(ERHIImageFormat::R32G32B32A32_UInt, "R32G32B32A32_UInt");

    // 注册枚举成员: R32G32B32A32_SInt
    Type->RegisterEnumMember(ERHIImageFormat::R32G32B32A32_SInt, "R32G32B32A32_SInt");

    // 注册枚举成员: R32G32B32A32_SFloat
    Type->RegisterEnumMember(ERHIImageFormat::R32G32B32A32_SFloat, "R32G32B32A32_SFloat");

    // 注册枚举成员: D16_UNorm
    Type->RegisterEnumMember(ERHIImageFormat::D16_UNorm, "D16_UNorm");

    // 注册枚举成员: D32_SFloat
    Type->RegisterEnumMember(ERHIImageFormat::D32_SFloat, "D32_SFloat");

    // 注册枚举成员: S8_UInt
    Type->RegisterEnumMember(ERHIImageFormat::S8_UInt, "S8_UInt");

    // 注册枚举成员: D16_UNorm_S8_UInt
    Type->RegisterEnumMember(ERHIImageFormat::D16_UNorm_S8_UInt, "D16_UNorm_S8_UInt");

    // 注册枚举成员: D24_UNorm_S8_UInt
    Type->RegisterEnumMember(ERHIImageFormat::D24_UNorm_S8_UInt, "D24_UNorm_S8_UInt");

    // 注册枚举成员: D32_SFloat_S8_UInt
    Type->RegisterEnumMember(ERHIImageFormat::D32_SFloat_S8_UInt, "D32_SFloat_S8_UInt");

    // 注册枚举成员: Count
    Type->RegisterEnumMember(ERHIImageFormat::Count, "Count");

}

#pragma warning(disable: 4100)  // 禁用未使用参数警告

void Z_Register_ERHIImageFormat()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<ERHIImageFormat>(Z_Register_ERHIImageFormat_Impl);
}
