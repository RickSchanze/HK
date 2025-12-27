#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FTextureImportSetting                                                                                        \
    struct Z_TextureImportSetting_Register                                                                                            \
    {                                                                                                                  \
        Z_TextureImportSetting_Register()                                                                                             \
        {                                                                                                              \
            Register_FTextureImportSetting();                                                                                         \
        }                                                                                                              \
        static void Register_FTextureImportSetting();                                                                                 \
    };                                                                                                                 \
    typedef FAssetImportSetting::ThisClass Super;                                                                                        \
    typedef FTextureImportSetting ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<FTextureImportSetting>(); }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FTextureImportSetting)                                                                                        \
    static void Register_FTextureImportSetting_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FTextureImportSetting::GPUFormat, "GPUFormat");                                                                                        \
    }                                                                                        \
    static inline Z_TextureImportSetting_Register Z_REGISTERER_TEXTUREIMPORTSETTING;

#define GENERATED_HEADER_HTexture                                                                                        \
    struct Z_HTexture_Register                                                                                            \
    {                                                                                                                  \
        Z_HTexture_Register()                                                                                             \
        {                                                                                                              \
            Register_HTexture();                                                                                         \
        }                                                                                                              \
        static void Register_HTexture();                                                                                 \
    };                                                                                                                 \
    typedef HAsset::ThisClass Super;                                                                                        \
    typedef HTexture ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<HTexture>(); }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(HTexture)                                                                                        \
    static inline Z_HTexture_Register Z_REGISTERER_HTEXTURE;
