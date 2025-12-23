#pragma once
#include "Asset.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Utility/UniquePtr.h"
#include "Core/Utility/Uuid.h"

#include "AssetRegistry.generated.h"
#include "Core/Container/LruCache.h"

HCLASS()
class HK_API FAssetImportSetting
{
    GENERATED_BODY(FAssetImportSetting)
public:
    virtual ~FAssetImportSetting() = default;
};

HSTRUCT()
struct HK_API FAssetMetaData
{
    GENERATED_BODY(FAssetMetaData)
public:
    HPROPERTY()
    FUuid Uuid;

    HPROPERTY()
    FString Path;

    HPROPERTY()
    EAssetType AssetType = EAssetType::Count;

    HPROPERTY()
    TUniquePtr<FAssetImportSetting> ImportSetting;
};

class FAssetRegistry : public FSingleton<FAssetRegistry>
{
public:
    FAssetRegistry() : CachedMetadata(512) {}

    FAssetMetaData& LoadAssetMetadata(const FString& Path);
    FAssetMetaData& LoadAssetMetadata(const FUuid Uuid);

    // 保存元数据（Metadata 必须在 LRUCache 中）
    bool SaveAssetMetadata(FAssetMetaData& Metadata);
    
    // 通过路径保存（会查找缓存中的 Metadata）
    bool SaveAssetMetadata(FString Path);
    
    // 通过 UUID 保存（会查找缓存中的 Metadata）
    bool SaveAssetMetadata(const FUuid Uuid);

private:
    TMap<FUuid, FString> UuidToPath;
    TMap<FString, FUuid> PathToUuid;
    TLruCache<FUuid, FAssetMetaData> CachedMetadata;
};
