#pragma once
#include "Core/Reflection/Reflection.h"
#include "Math/Vector.h"
#include "Object/Asset.h"
#include "RHI/RHIImage.h"
#include "RHI/RHIImageView.h"

#include "RHI/RHISampler.h"
#include "Texture.generated.h"

HCLASS()
class HTexture : public HAsset
{
    GENERATED_BODY(HTexture)

public:
    HTexture();

    ~HTexture() override;

    // 内部设置方法（由 Importer 使用）
    void internalSetRHIImage(const FRHIImage& InImage)
    {
        Image = InImage;
    }

    void internalSetRHIImageView(const FRHIImageView& InImageView)
    {
        ImageView = InImageView;
    }

    void internalSetWidth(Int32 InWidth)
    {
        Width = InWidth;
    }

    void internalSetHeight(Int32 InHeight)
    {
        Height = InHeight;
    }

    void internalSetFormat(ERHIImageFormat InFormat)
    {
        Format = InFormat;
    }

    // 公共访问方法
    const FRHIImage& GetRHIImage() const
    {
        return Image;
    }

    const FRHIImageView& GetRHIImageView() const
    {
        return ImageView;
    }

    Int32 GetWidth() const
    {
        return Width;
    }

    Int32 GetHeight() const
    {
        return Height;
    }

    ERHIImageFormat GetFormat() const
    {
        return Format;
    }

    FVector2i GetSize() const
    {
        return {Width, Height};
    }

    TEvent<HTexture*>& GetPreDestroyEvent()
    {
        return PreDestroyEvent;
    }

private:
    FRHIImage       Image;
    FRHIImageView   ImageView;
    Int32           Width  = 0;
    Int32           Height = 0;
    ERHIImageFormat Format = ERHIImageFormat::Undefined;

    TEvent<HTexture*> PreDestroyEvent;
};