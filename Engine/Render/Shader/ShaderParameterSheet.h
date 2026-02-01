#pragma once
#include "Core/Reflection/Reflection.h"

#include "ShaderParameterSheet.generated.h"

HSTRUCT()
struct FShaderPushConstantItem
{
    GENERATED_BODY(FShaderPushConstantItem)
public:
    HPROPERTY()
    FName Name;

    HPROPERTY()
    UInt32 Offset = 0;

    HPROPERTY()
    UInt32 Size = 0;
};

struct FShaderNames
{
    static inline auto ModelID            = FName("ModelID");
    static inline auto MainTextureID      = FName("MainTextureID");
    static inline auto MainSamplerStateID = FName("MainSamplerStateID");
};

HSTRUCT()
struct FShaderParameterSheet
{
    GENERATED_BODY(FShaderParameterSheet)
public:
    // Shader是否声明相机参数
    HPROPERTY()
    bool bNeedCamera{};

    // Shader是否声明模型参数
    HPROPERTY()
    bool bNeedModel{};

    // Shader是否声明资源池参数
    HPROPERTY()
    bool bNeedResourcePool{};

    // Shader的PushConstant参数
    HPROPERTY()
    TArray<FShaderPushConstantItem> PushConstants;

    // 参数是否有效
    bool bIsValid = false;
};

HSTRUCT()
struct FShaderBinaryData
{
    GENERATED_BODY(FShaderBinaryData)
public:
    HPROPERTY()
    FShaderParameterSheet ParameterSheet;

    HPROPERTY()
    TArray<UInt32> VS;

    HPROPERTY()
    TArray<UInt32> FS;
};
