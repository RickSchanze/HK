//
// Created by Admin on 2026/1/4.
//

#include "AssetLoader.h"

#include "AssetRegistry.h"

HObject* FAssetLoader::Load(FStringView AssetPath, FType AssetType, bool ImportIfNotExist)
{
    auto Meta = FAssetRegistry::GetRef().LoadAssetMetadata(AssetPath);
    if (!Meta)
    {
        return nullptr;
    }
    return Load(*Meta, AssetType, ImportIfNotExist);
}

HObject* FAssetLoader::Load(FUuid AssetId, FType AssetType, bool ImportIfNotExist)
{
    auto Meta = FAssetRegistry::GetRef().LoadAssetMetadata(AssetId);
    if (!Meta)
    {
        return nullptr;
    }
    return Load(*Meta, AssetType, ImportIfNotExist);
}
