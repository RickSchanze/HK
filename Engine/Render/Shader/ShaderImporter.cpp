//
// Created by Admin on 2025/12/26.
//

#include "ShaderImporter.h"

#include "Core/Container/Array.h"
#include "Core/Logging/Logger.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/BinaryArchive.h"
#include "Core/Serialization/MemoryStream.h"
#include "Core/String/String.h"
#include "Core/Utility/FileUtility.h"
#include "Core/Utility/HashUtility.h"
#include "Core/Utility/Profiler.h"
#include "Core/Utility/SharedPtr.h"
#include "Core/Utility/Uuid.h"
#include "Object/AssetImporter.h"
#include "Object/AssetManager.h"
#include "Object/AssetRegistry.h"
#include "Object/AssetUtility.h"
#include "Object/Object.h"
#include "Render/Shader/Shader.h"
#include "Render/Shader/SlangCompiler.h"

void FShaderImporter::BeginImport()
{
    // 分配导入数据
    ImportData = New<FImportData>();
}

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
        HK_LOG_ERROR(ELogcat::Asset, "Failed to get import setting for shader: {}", Metadata->Path);
        return false;
    }

    // 编译 Shader
    FShaderCompileRequest Request;
    Request.ShaderPath = Metadata->Path;
    Request.Target     = EShaderCompileTarget::Spirv;

    if (!FSlangCompiler::GetRef().RequestCompileGraphicsShader(Request, ImportData->CompileResult))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to compile shader: {} - {}", Metadata->Path,
                    ImportData->CompileResult.ErrorMessage);
        return false;
    }

    // 检查编译结果是否有错误
    if (!ImportData->CompileResult.ErrorMessage.IsEmpty())
    {
        HK_LOG_WARN(ELogcat::Asset, "Shader compilation warnings: {} - {}", Metadata->Path,
                   ImportData->CompileResult.ErrorMessage);
    }

    // 创建 HShader 对象
    FObjectArray& ObjectArray = FObjectArray::GetRef();
    ImportData->Shader         = ObjectArray.CreateObject<HShader>(FName(Metadata->Path));
    if (!ImportData->Shader)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create HShader object");
        return false;
    }

    // 设置编译结果到 Shader
    ImportData->Shader->SetCompileResult(ImportData->CompileResult);

    // 保存元数据
    Metadata->AssetType = EAssetType::Shader;
    FAssetRegistry::GetRef().SaveAssetMetadata(Metadata);

    HK_LOG_INFO(ELogcat::Asset, "Successfully processed shader import: {}", Metadata->Path);
    return true;
}

bool FShaderImporter::ProcessAssetIntermediate()
{
    if (!Metadata || !ImportData)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Metadata or ImportData is null in ProcessAssetIntermediate");
        return false;
    }

    // 如果还没有编译结果，先编译
    if (ImportData->CompileResult.VS.IsEmpty() && ImportData->CompileResult.FS.IsEmpty())
    {
        FShaderCompileRequest Request;
        Request.ShaderPath = Metadata->Path;
        Request.Target     = EShaderCompileTarget::Spirv;

        if (!FSlangCompiler::GetRef().RequestCompileGraphicsShader(Request, ImportData->CompileResult))
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to compile shader for intermediate: {} - {}", Metadata->Path,
                        ImportData->CompileResult.ErrorMessage);
            return false;
        }
    }

    // 获取中间文件路径
    FString IntermediatePath = FAssetUtility::GetShaderIntermediatePath(Metadata->Uuid);

    // 构建中间数据结构（先不设置 Hash）
    FShaderIntermediate Intermediate;
    Intermediate.BinaryData.ParameterSheet = ImportData->CompileResult.ParameterSheet;
    Intermediate.BinaryData.VS              = ImportData->CompileResult.VS;
    Intermediate.BinaryData.FS              = ImportData->CompileResult.FS;

    // 先序列化以计算 Hash（此时 Hash 字段为 0），直接计算不存储
    FHashOutputStream HashStream;
    {
        FBinaryOutputArchive HashAr(HashStream);
        HashAr(Intermediate);
    }

    // 获取计算出的 Hash（不包括 Hash 字段本身，因为此时 Hash 为 0）
    const UInt64 Hash = HashStream.GetHash();

    // 设置 Hash 到 Intermediate
    Intermediate.Hash = Hash;

    // 创建文件流并序列化
    auto Stream = FFileUtility::CreateFileStream(IntermediatePath, true, true);
    if (!Stream)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create file stream for intermediate file: {}", IntermediatePath);
        return false;
    }

    FBinaryOutputArchive Ar(*Stream);
    Intermediate.Serialize(Ar);

    // 更新 Metadata 中的 Hash
    Metadata->IntermediateHash = Hash;
    FAssetRegistry::GetRef().SaveAssetMetadata(Metadata);

    HK_LOG_INFO(ELogcat::Asset, "Saved intermediate shader data to: {} (Hash: {})", IntermediatePath, Hash);
    return true;
}

void FShaderImporter::EndImport(const bool Success)
{
    if (!ImportData)
    {
        return;
    }
    if (Success)
    {
        FAssetManager::GetRef().RegisterAsset(Metadata->Uuid, Metadata->Path, ImportData->Shader);
    }

    // 删除导入数据
    Delete(ImportData);
    ImportData = nullptr;
}