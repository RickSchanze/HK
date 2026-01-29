#pragma once
#include "Core/Container/Array.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/String.h"
#include "ShaderParameterSheet.h"

#include "SlangTranslator.generated.h"

HSTRUCT()
struct FShaderTranslateResult
{
    GENERATED_BODY(FShaderTranslateResult)
public:
    FString ErrorMessage;

    HPROPERTY()
    FShaderParameterSheet ParameterSheet;

    HPROPERTY()
    TArray<UInt32> VS; // 顶点着色器代码

    HPROPERTY()
    TArray<UInt32> FS; // 片段着色器代码

    bool IsValid() const
    {
        return !FS.IsEmpty() && !VS.IsEmpty() && ErrorMessage.IsEmpty();
    }

    void ClearAllCode()
    {
        VS = {};
        FS = {};
    }
};

enum class EShaderTranslateTarget
{
    GLSL,
    HLSL,
    Spirv,
    Count,
};

struct FShaderTranslatorRequest
{
    FString                ShaderPath;
    EShaderTranslateTarget Target = EShaderTranslateTarget::Spirv;

    EShaderTranslateTarget DebugOutputTarget = EShaderTranslateTarget::Count;
    FString                DebugOutputPath;
};

class FSlangTranslator : public TSingleton<FSlangTranslator>
{
public:
    void StartUp() override;
    void ShutDown() override;

    bool RequestCompileGraphicsShader(const FShaderTranslatorRequest& Request, FShaderTranslateResult& OutResult);

    class FImpl;

private:
    FImpl* Impl = nullptr;
};
