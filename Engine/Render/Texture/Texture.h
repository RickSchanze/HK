#pragma once
#include "Core/Reflection/Reflection.h"
#include "Object/Asset.h"
#include "Object/AssetRegistry.h"
#include "RHI/RHIImage.h"
#include "RHI/RHIImageView.h"

#include "Texture.generated.h"

HCLASS()
class FTextureImportSetting : public FAssetImportSetting
{
    GENERATED_BODY(FTextureImportSetting)
public:
    HPROPERTY()
    ERHIImageFormat GPUFormat = ERHIImageFormat::B8G8R8A8_SRGB;
};

HCLASS()
class HTexture : HAsset
{
    GENERATED_BODY(HTexture)
public:
    ~HTexture() override;


    void internalSetRHIImage(const FRHIImage& InImage)
    {
        Image = InImage;
    }
    void internalSetRHIImageView(const FRHIImageView& InImageView)
    {
        ImageView = InImageView;
    }

private:
    FRHIImage     Image;
    FRHIImageView ImageView;
};