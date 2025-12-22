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
    static inline Z_HAsset_Register Z_REGISTERER_HASSET;
