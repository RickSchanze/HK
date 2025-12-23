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

    EAssetType GetAssetType() const
    {
        return AssetType;
    }

private:
    HPROPERTY()
    EAssetType AssetType = EAssetType::Count;
};