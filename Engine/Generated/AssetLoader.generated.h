#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FAssetLoader                                                                                        \
    struct Z_AssetLoader_Register                                                                                            \
    {                                                                                                                  \
        Z_AssetLoader_Register()                                                                                             \
        {                                                                                                              \
            Register_FAssetLoader();                                                                                         \
        }                                                                                                              \
        static void Register_FAssetLoader();                                                                                 \
    };                                                                                                                 \
    typedef FAssetLoader ThisClass;                                                                                        \
    virtual FType GetType() const { return TypeOf<FAssetLoader>(); }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FAssetLoader)                                                                                        \
    static inline Z_AssetLoader_Register Z_REGISTERER_ASSETLOADER;
