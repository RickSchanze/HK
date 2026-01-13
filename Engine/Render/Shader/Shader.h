#pragma once
#include "Object/Asset.h"

#include "Shader.generated.h"
#include "SlangCompiler.h"

HCLASS()
class HShader : public HAsset
{
    GENERATED_BODY(HShader)
public:
    HShader()
    {
        AssetType = EAssetType::Shader;
    }

    void SetCompileResult(const FShaderCompileResult& Result)
    {
        ShaderCompileResult = Result;
    }

    const FShaderCompileResult& GetCompileResult() const
    {
        return ShaderCompileResult;
    }

private:
    FShaderCompileResult ShaderCompileResult;
};
