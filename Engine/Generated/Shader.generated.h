#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_HShader                                                                                        \
    struct Z_HShader_Register                                                                                            \
    {                                                                                                                  \
        Z_HShader_Register()                                                                                             \
        {                                                                                                              \
            Register_HShader();                                                                                         \
        }                                                                                                              \
        static void Register_HShader();                                                                                 \
    };                                                                                                                 \
    typedef HAsset::ThisClass Super;                                                                                        \
    typedef HShader ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<HShader>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(HShader)                                                                                        \
    static inline Z_HShader_Register Z_REGISTERER_HSHADER;
