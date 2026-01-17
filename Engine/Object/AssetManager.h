#pragma once
#include "Core/Container/Map.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/String.h"
#include "Core/Utility/Profiler.h"
#include "Core/Utility/Uuid.h"
#include "Object.h"

class FAssetManager : public TSingleton<FAssetManager>
{
public:
    HObject* LoadAsset(FStringView AssetPath);

    template <typename T>
    T* LoadAsset(FStringView AssetPath)
    {
    }

    void RegisterAsset(FUuid Uuid, const FString& AssetPath, const HObject* Asset);

private:
    HK_PROFILE_LOCKABLE(std::mutex, AssetMapMutex);
    TMap<FUuid, FObjectID> AssetMap;
    TMap<FString, FUuid>   AssetPathMap;
};