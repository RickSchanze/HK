#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

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
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(HTexture)                                                                                        \
    static inline Z_HTexture_Register Z_REGISTERER_HTEXTURE;
