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

#define GENERATED_HEADER_FTextureIntermediate                                                                                        \
    struct Z_TextureIntermediate_Register                                                                                            \
    {                                                                                                                  \
        Z_TextureIntermediate_Register()                                                                                             \
        {                                                                                                              \
            Register_FTextureIntermediate();                                                                                         \
        }                                                                                                              \
        static void Register_FTextureIntermediate();                                                                                 \
    };                                                                                                                 \
    typedef FTextureIntermediate ThisStruct;                                                                                        \
    static FType GetType() { return TypeOf<FTextureIntermediate>(); }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("Hash", Hash),                                                                                     \
            MakeNamedPair("Width", Width),                                                                                     \
            MakeNamedPair("Height", Height),                                                                                     \
            MakeNamedPair("Format", Format),                                                                                     \
            MakeNamedPair("ImageData", ImageData)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FTextureIntermediate_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FTextureIntermediate::Hash, "Hash");                                                                                        \
        Type->RegisterProperty(&FTextureIntermediate::Width, "Width");                                                                                        \
        Type->RegisterProperty(&FTextureIntermediate::Height, "Height");                                                                                        \
        Type->RegisterProperty(&FTextureIntermediate::Format, "Format");                                                                                        \
        Type->RegisterProperty(&FTextureIntermediate::ImageData, "ImageData");                                                                                        \
    }                                                                                        \
    static inline Z_TextureIntermediate_Register Z_REGISTERER_TEXTUREINTERMEDIATE;
