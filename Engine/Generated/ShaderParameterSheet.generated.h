#pragma once

#include "Core/Utility/Macros.h"

#define GENERATED_HEADER_FShaderPushConstantItem                                                                                        \
    struct Z_ShaderPushConstantItem_Register                                                                                            \
    {                                                                                                                  \
        Z_ShaderPushConstantItem_Register()                                                                                             \
        {                                                                                                              \
            Register_FShaderPushConstantItem();                                                                                         \
        }                                                                                                              \
        static void Register_FShaderPushConstantItem();                                                                                 \
    };                                                                                                                 \
    typedef FShaderPushConstantItem ThisStruct;                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("Name", Name),                                                                                     \
            MakeNamedPair("Offset", Offset),                                                                                     \
            MakeNamedPair("Size", Size)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FShaderPushConstantItem_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FShaderPushConstantItem::Name, "Name");                                                                                        \
        Type->RegisterProperty(&FShaderPushConstantItem::Offset, "Offset");                                                                                        \
        Type->RegisterProperty(&FShaderPushConstantItem::Size, "Size");                                                                                        \
    }                                                                                        \
    static inline Z_ShaderPushConstantItem_Register Z_REGISTERER_SHADERPUSHCONSTANTITEM;

#define GENERATED_HEADER_FShaderParameterSheet                                                                                        \
    struct Z_ShaderParameterSheet_Register                                                                                            \
    {                                                                                                                  \
        Z_ShaderParameterSheet_Register()                                                                                             \
        {                                                                                                              \
            Register_FShaderParameterSheet();                                                                                         \
        }                                                                                                              \
        static void Register_FShaderParameterSheet();                                                                                 \
    };                                                                                                                 \
    typedef FShaderParameterSheet ThisStruct;                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("bNeedCamera", bNeedCamera),                                                                                     \
            MakeNamedPair("bNeedModel", bNeedModel),                                                                                     \
            MakeNamedPair("bNeedResourcePool", bNeedResourcePool)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FShaderParameterSheet_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FShaderParameterSheet::bNeedCamera, "bNeedCamera");                                                                                        \
        Type->RegisterProperty(&FShaderParameterSheet::bNeedModel, "bNeedModel");                                                                                        \
        Type->RegisterProperty(&FShaderParameterSheet::bNeedResourcePool, "bNeedResourcePool");                                                                                        \
    }                                                                                        \
    static inline Z_ShaderParameterSheet_Register Z_REGISTERER_SHADERPARAMETERSHEET;
