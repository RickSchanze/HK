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
    // 重写基类方法
    void BeginImport() override;
    bool ProcessImport() override;
    bool ProcessAssetIntermediate() override;
    void EndImport() override;

private:
    // 导入过程中的临时数据
    struct FImportData
    {
        FImageData        ImageData;
        FRHIBuffer       StagingBuffer;
        FRHICommandBuffer CommandBuffer;
        FRHIImage        Image;
        FRHIImageView    ImageView;
        HTexture*        Texture = nullptr;
        ERHIImageFormat  ImageFormat = ERHIImageFormat::Undefined;
    };

    FImportData* ImportData = nullptr;
};
