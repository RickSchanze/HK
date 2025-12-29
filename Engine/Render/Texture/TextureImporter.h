#pragma once
#include "Core/Container/Array.h"
#include "Object/AssetImporter.h"
#include "RHI/RHIImage.h"

#include "RHI/RHICommandBuffer.h"
#include "RHI/RHIImageView.h"
#include "TextureImporter.generated.h"

class HTexture;

// 使用 stb_image 加载的图像数据
struct FImageData
{
    UInt8* Data     = nullptr;
    Int32  Width    = 0;
    Int32  Height   = 0;
    Int32  Channels = 0;
    bool   bIsHDR   = false;
};

HCLASS()
class FTextureImportSetting : public FAssetImportSetting
{
    GENERATED_BODY(FTextureImportSetting)
public:
    HPROPERTY()
    ERHIImageFormat GPUFormat = ERHIImageFormat::B8G8R8A8_SRGB;
};

// 纹理中间数据结构
HSTRUCT()
struct FTextureIntermediate
{
    GENERATED_BODY(FTextureIntermediate)
public:
    HPROPERTY()
    UInt64 Hash = 0; // Hash 值（计算时排除此字段）

    HPROPERTY()
    Int32 Width = 0;

    HPROPERTY()
    Int32 Height = 0;

    HPROPERTY()
    ERHIImageFormat Format = ERHIImageFormat::Undefined;

    HPROPERTY()
    TArray<UInt8> ImageData;
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
        FRHIBuffer        StagingBuffer;
        FRHICommandBuffer CommandBuffer;
        FRHIImage         Image;
        FRHIImageView     ImageView;
        HTexture*         Texture     = nullptr;
        ERHIImageFormat   ImageFormat = ERHIImageFormat::Undefined;
    };

    FImportData* ImportData = nullptr;
};
