#pragma once
#include "Core/Utility/Uuid.h"
#include "Object.h"

#include "Asset.generated.h"

HENUM()
enum class EAssetType
{
    Shader,
    Mesh,
    Texture,
    Count,
};

HCLASS()
class HK_API HAsset : public HObject
{
    GENERATED_BODY(HAsset)
public:
    HAsset() : Super(EObjectFlags::Asset) {}

    FUuid GetUuid() const
    {
        return Uuid;
    }

    EAssetType GetAssetType() const
    {
        return AssetType;
    }

private:
    HPROPERTY()
    FUuid Uuid;

    HPROPERTY()
    FString AssetPath;

    HPROPERTY()
    EAssetType AssetType = EAssetType::Count;
};