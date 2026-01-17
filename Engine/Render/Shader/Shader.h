#pragma once
#include "Object/Asset.h"

#include "Core/Container/FixedArray.h"
#include "RHI/RHIPipeline.h"
#include "Shader.generated.h"
#include "SlangTranslator.h"

HCLASS()
class HShader : public HAsset
{
    GENERATED_BODY(HShader)
public:
    HShader()
    {
        AssetType = EAssetType::Shader;
    }

    void SetCompileResult(const FShaderTranslateResult& Result)
    {
        ShaderCompileResult = Result;
    }

    const FShaderTranslateResult& GetCompileResult() const
    {
        return ShaderCompileResult;
    }

    /**
     * 从FShaderCompileResult编译RHIShaderModule
     * @param OutShaderModules 输出RHIShaderModule
     * @param ClearCode 是否清除ShaderCompileResult中的代码
     */
    bool Compile(TFixedArray<FRHIShaderModule, 2>& OutShaderModules, bool ClearCode = true);

private:
    FShaderTranslateResult ShaderCompileResult;

    bool IsCompiled = false;
};
