#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_HMaterial                                                                                        \
    struct Z_HMaterial_Register                                                                                            \
    {                                                                                                                  \
        Z_HMaterial_Register()                                                                                             \
        {                                                                                                              \
            Register_HMaterial();                                                                                         \
        }                                                                                                              \
        static void Register_HMaterial();                                                                                 \
    };                                                                                                                 \
    typedef HAsset::ThisClass Super;                                                                                        \
    typedef HMaterial ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<HMaterial>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(HMaterial)                                                                                        \
    static void Register_HMaterial_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&HMaterial::Shader, "Shader");                                                                                        \
    }                                                                                        \
    static inline Z_HMaterial_Register Z_REGISTERER_HMATERIAL;
