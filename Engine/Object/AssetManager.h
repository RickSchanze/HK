#pragma once
#include "Core/Container/Map.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/String.h"
#include "Core/Utility/Profiler.h"
#include "Core/Utility/Uuid.h"

class HObject;
class FAssetManager : public FSingleton<FAssetManager>
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
    TMap<FUuid, int>     AssetMap;
    TMap<FString, FUuid> AssetPathMap;
};