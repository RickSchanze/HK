//
// Created by Admin on 2025/12/26.
//

#include "ShaderImporter.h"

#include "Object/AssetUtility.h"
#include "SlangCompiler.h"

void FShaderImporter::BeginImport()
{
    FAssetImporter::BeginImport();

}

bool FShaderImporter::ProcessAssetIntermediate()
{
    FShaderCompileRequest Request;
    Request.ShaderPath = Metadata->Path;
    Request.Target = EShaderCompileTarget::Spirv;

    FShaderCompileResult Result;
    FSlangCompiler::GetRef().RequestCompileGraphicsShader(Request, Result);

}

// 只处理元数据, 管线对象交给Material
bool FShaderImporter::ProcessImport()
{
    if (!Metadata)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Metadata is null in ProcessImport");
        return false;
    }

    // 获取或创建导入设置
    if (!Metadata->ImportSetting)
    {
        Metadata->ImportSetting = MakeShared<FShaderImportSetting>();
    }

    auto ShaderSetting = DynamicPointerCast<FShaderImportSetting>(Metadata->ImportSetting);
    if (!ShaderSetting)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to get import setting for mesh: {}", Metadata->Path);
        return false;
    }

    FAssetRegistry::GetRef().SaveAssetMetadata(Metadata);
    return true;
}

void FShaderImporter::EndImport(bool Success)
{
    FAssetImporter::EndImport(Success);
}