#pragma once
#include "Core/Container/Array.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/String.h"
#include "ShaderParameterSheet.h"

#include "SlangCompiler.generated.h"

HSTRUCT()
struct FShaderCompileResult
{
    GENERATED_BODY(FShaderCompileResult)
public:
    FString ErrorMessage;

    HPROPERTY()
    FShaderParameterSheet ParameterSheet;

    HPROPERTY()
    TArray<UInt32> VS;  // 顶点着色器代码

    HPROPERTY()
    TArray<UInt32> FS;  // 片段着色器代码
};

enum class EShaderCompileTarget
{
    GLSL,
    HLSL,
    Spirv,
    Count,
};

struct FShaderCompileRequest
{
    FString ShaderPath;
    EShaderCompileTarget Target = EShaderCompileTarget::Spirv;

    EShaderCompileTarget DebugOutputTarget = EShaderCompileTarget::Count;
    FString DebugOutputPath;
};

class FSlangCompiler : public FSingleton<FSlangCompiler>
{
public:
    void StartUp() override;
    void ShutDown() override;

    bool RequestCompileGraphicsShader(const FShaderCompileRequest& Request, FShaderCompileResult& OutResult);

    class FImpl;

private:
    FImpl* Impl = nullptr;
};
