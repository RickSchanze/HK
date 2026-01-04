#pragma once

#include "Object/AssetLoader.h"

class HMesh;

/**
 * Mesh 加载器，负责从 Intermediate 文件加载 Mesh 或触发 Import
 */
class HK_API FMeshLoader : public FAssetLoader
{
public:
    HObject* Load(const FAssetMetadata& Metadata, FType AssetType, bool ImportIfNotExist) override;
};

