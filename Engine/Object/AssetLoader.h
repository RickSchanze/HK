#pragma once

#include "Core/Reflection/Reflection.h"
#include "Core/Utility/Uuid.h"

#include "AssetLoader.generated.h"

struct FAssetMetadata;
class HObject;

HCLASS()
class FAssetLoader
{
    GENERATED_BODY(FAssetLoader)
public:
    virtual ~FAssetLoader() = default;

    virtual HObject* Load(const FAssetMetadata& Metadata, FType AssetType, bool ImportIfNotExist) = 0;

    template <typename T>
    T* Load(const FAssetMetadata& Metadata, bool ImportIfNotExist)
    {
        return static_cast<T*>(Load(Metadata, TypeOf<T>(), ImportIfNotExist));
    }


    HObject* Load(FStringView AssetPath, FType AssetType, bool ImportIfNotExist);

    template <typename T>
    T* Load(FStringView AssetPath, bool ImportIfNotExist)
    {
        return static_cast<T*>(Load(AssetPath, TypeOf<T>(), ImportIfNotExist));
    }


    HObject* Load(FUuid AssetId, FType AssetType, bool ImportIfNotExist);

    template <typename T>
    T* Load(FUuid AssetId, bool ImportIfNotExist)
    {
        return static_cast<T*>(Load(AssetId, TypeOf<T>(), ImportIfNotExist));
    }
};
