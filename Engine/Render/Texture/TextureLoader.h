#pragma once

#include "Object/AssetLoader.h"
#include "Object/AssetUtility.h"

class HTexture;

/**
 * 纹理加载器，负责从 Intermediate 文件加载纹理或触发 Import
 */
class HK_API FTextureLoader : public FAssetLoader
{
public:
    HObject* Load(const FAssetMetadata& Metadata, FType AssetType, bool ImportIfNotExist) override;
};

