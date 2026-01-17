//
// Created by Admin on 2026/1/5.
//

#include "ShaderLoader.h"
#include "Core/Logging/Logger.h"
#include "Core/Serialization/BinaryArchive.h"
#include "Core/Utility/FileUtility.h"
#include "Object/AssetImporter.h"
#include "Object/AssetRegistry.h"
#include "Object/Object.h"
#include "Render/Shader/Shader.h"
#include "Render/Shader/ShaderImporter.h"
#include <fstream>

namespace
{
// 从 Intermediate 文件加载 Shader
HShader* LoadShaderFromIntermediate(const FAssetMetadata& Metadata)
{
    // 获取中间文件路径
    FString IntermediatePath = FAssetUtility::GetShaderIntermediatePath(Metadata.Uuid);

    // 打开文件流
    auto FileStream = FFileUtility::OpenFileStream(IntermediatePath, true);
    if (!FileStream)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to open intermediate file: {}", IntermediatePath);
        return nullptr;
    }

    // 反序列化 Intermediate 数据
    FShaderIntermediate Intermediate;
    {
        FBinaryInputArchive Ar(*FileStream);
        Ar(Intermediate);
    }

    // 创建 HShader 对象
    FObjectArray& ObjectArray = FObjectArray::GetRef();
    HShader*      Shader      = ObjectArray.CreateObject<HShader>(FName(Metadata.Path));
    if (!Shader)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Failed to create HShader object");
        return nullptr;
    }

    // 将 FShaderBinaryData 转换为 FShaderCompileResult
    FShaderTranslateResult CompileResult;
    CompileResult.ErrorMessage   = FString(); // 从 Intermediate 加载时没有错误信息
    CompileResult.ParameterSheet = Intermediate.BinaryData.ParameterSheet;
    CompileResult.VS             = Intermediate.BinaryData.VS;
    CompileResult.FS             = Intermediate.BinaryData.FS;

    // 设置编译结果到 Shader
    Shader->SetCompileResult(CompileResult);

    HK_LOG_INFO(ELogcat::Asset, "Successfully loaded shader from intermediate: {}", Metadata.Path);
    return Shader;
}
} // namespace

HObject* FShaderLoader::Load(const FAssetMetadata& Metadata, FType AssetType, bool ImportIfNotExist)
{
    // 校验类型
    if (AssetType != TypeOf<HShader>())
    {
        HK_LOG_ERROR(ELogcat::Asset, "Invalid asset type for ShaderLoader: {}", AssetType->Name);
        return nullptr;
    }

    // 获取中间文件路径
    const FString IntermediatePath = FAssetUtility::GetShaderIntermediatePath(Metadata.Uuid);

    // 校验 Hash

    if (const TSharedPtr<FAssetMetadata> MetaPtr = MakeShared<FAssetMetadata>(Metadata);
        FAssetUtility::ValidateIntermediateHash(MetaPtr, IntermediatePath))
    {
        // Hash 校验通过，直接从 Intermediate 加载
        if (HShader* Shader = LoadShaderFromIntermediate(Metadata))
        {
            return Shader;
        }
        HK_LOG_WARN(ELogcat::Asset, "Failed to load shader from intermediate, will try import if allowed");
    }

    // Hash 校验失败或加载失败，如果允许 Import 则进行 Import
    if (ImportIfNotExist)
    {
        // 创建 Importer 并执行 Import
        const TSharedPtr<FAssetMetadata> ImportMeta = FAssetUtility::GetOrCreateAssetMetadata(Metadata.Path);
        if (!ImportMeta)
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to get or create asset metadata for import: {}", Metadata.Path);
            return nullptr;
        }

        if (FShaderImporter Importer; !Importer.Import(ImportMeta))
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to import shader: {}", Metadata.Path);
            return nullptr;
        }

        // Import 完成后，从 ObjectArray 中获取准备好的 Shader
        HShader* Shader = FObjectArray::GetRef().FindObjectByName<HShader>(FName(Metadata.Path));
        if (!Shader)
        {
            HK_LOG_ERROR(ELogcat::Asset, "Failed to get shader from ObjectArray after import: {}", Metadata.Path);
            return nullptr;
        }

        return Shader;
    }

    HK_LOG_ERROR(ELogcat::Asset, "Failed to load shader and ImportIfNotExist is false: {}", Metadata.Path);
    return nullptr;
}