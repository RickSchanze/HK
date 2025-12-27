//
// Created by Admin on 2025/12/26.
//

#include "Texture.h"

#include "RHI/GfxDevice.h"

HTexture::HTexture()
{
    AssetType = EAssetType::Texture;
}

HTexture::~HTexture()
{
    if (ImageView)
    {
        GetGfxDeviceRef().DestroyImageView(ImageView);
    }
    if (Image)
    {
        GetGfxDeviceRef().DestroyImage(Image);
    }
}
