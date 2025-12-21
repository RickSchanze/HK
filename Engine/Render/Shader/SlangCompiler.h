#pragma once
#include "Core/Container/Array.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/String.h"
#include "ShaderParameterSheet.h"

struct FShaderCompileResult
{
    FString ErrorMessage;
    TArray<UInt32> Code;
    FShaderParameterSheet ParameterSheet;
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

    bool RequestCompile(const FShaderCompileRequest& Request, FShaderCompileResult& OutResult);

    class FImpl;

private:
    FImpl* Impl = nullptr;
};
