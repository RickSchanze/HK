//
// Created by Admin on 2025/12/28.
//

#include "AssetManager.h"

#include "Object.h"

void FAssetManager::RegisterAsset(FUuid Uuid, const FString& AssetPath, const HObject* Asset)
{
    if (AssetMap.Contains(Uuid) || AssetPathMap.Contains(AssetPath))
    {
        HK_LOG_ERROR(ELogcat::Asset, "Asset {} already exists", Uuid.ToString());
        return;
    }
    if (Asset == nullptr)
    {
        HK_LOG_ERROR(ELogcat::Asset, "Asset {} is null, with UUID={}, AssetPath={}", Uuid, AssetPath);
        return;
    }
    AutoLock Lock(AssetMapMutex);
    AssetMap[Uuid]                   = Asset->GetID();
    AssetPathMap[FString(AssetPath)] = Uuid;
}