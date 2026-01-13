#pragma once
#include "Object/AssetImporter.h"

#include "ShaderImporter.generated.h"
#include "ShaderParameterSheet.h"
#include "SlangCompiler.h"

class HShader;

HCLASS()
class FShaderImportSetting : public FAssetImportSetting
{
    GENERATED_BODY(FShaderImportSetting)
};

// Shader中间数据结构
HSTRUCT()
struct FShaderIntermediate
{
    GENERATED_BODY(FShaderIntermediate)
public:
    HPROPERTY()
    UInt64 Hash = 0; // Hash 值（计算时排除此字段）

    HPROPERTY()
    FShaderBinaryData BinaryData;
};

class FShaderImporter : public FAssetImporter
{
public:
    // 重写基类方法
    void BeginImport() override;
    bool ProcessImport() override;
    bool ProcessAssetIntermediate() override;
    void EndImport(bool Success) override;

    // 导入过程中的临时数据
    struct FImportData
    {
        FShaderCompileResult CompileResult;
        HShader*             Shader = nullptr;
    };

private:
    FImportData* ImportData = nullptr;
};
