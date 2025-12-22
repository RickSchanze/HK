#pragma once

#include "Core/Utility/Macros.h"

#define GENERATED_HEADER_HAsset                                                                                        \
    struct Z_HAsset_Register                                                                                            \
    {                                                                                                                  \
        Z_HAsset_Register()                                                                                             \
        {                                                                                                              \
            Register_HAsset();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_HAsset();                                                                                 \
    };                                                                                                                 \
    typedef HObject::ThisClass Super;                                                                                        \
    typedef HAsset ThisClass;                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(HAsset)                                                                                        \
    static void Register_HAsset_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&HAsset::Uuid, "Uuid");                                                                                        \
        Type->RegisterProperty(&HAsset::AssetPath, "AssetPath");                                                                                        \
        Type->RegisterProperty(&HAsset::AssetType, "AssetType");                                                                                        \
    }                                                                                        \
    static inline Z_HAsset_Register Z_REGISTERER_HASSET;

HK_API void Z_Register_EAssetType();
struct F_Z_Register_EAssetType
{
    F_Z_Register_EAssetType()
    {
        Z_Register_EAssetType();
    }
};
static inline F_Z_Register_EAssetType Z_REGISTERER_EASSETTYPE;
