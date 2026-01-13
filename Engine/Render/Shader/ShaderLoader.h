#pragma once

#include "Object/AssetLoader.h"
#include "Object/AssetUtility.h"

class HShader;

/**
 * Shader 加载器，负责从 Intermediate 文件加载 Shader 或触发 Import
 */
class HK_API FShaderLoader : public FAssetLoader
{
public:
    HObject* Load(const FAssetMetadata& Metadata, FType AssetType, bool ImportIfNotExist) override;
};