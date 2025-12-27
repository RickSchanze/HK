#pragma once
#include "Object/AssetImporter.h"
#include "RHI/RHIImage.h"

#include "TextureImporter.generated.h"

HCLASS()
class FTextureImportSetting : public FAssetImportSetting
{
    GENERATED_BODY(FTextureImportSetting)
public:
    HPROPERTY()
    ERHIImageFormat GPUFormat = ERHIImageFormat::B8G8R8A8_SRGB;
};

class FTextureImporter : public FAssetImporter
{
public:
    bool Import(FStringView Path, EAssetFileType FileType, EAssetImportOptions Options) override;
    TSharedPtr<FAssetImportSetting> GetOrCreateImportSetting(FAssetMetaData& Metadata) override;
};
