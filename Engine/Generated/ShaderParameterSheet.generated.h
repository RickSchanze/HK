#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

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
    static FType GetType() { return TypeOf<FShaderPushConstantItem>(); }                                                                                        \
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
    static FType GetType() { return TypeOf<FShaderParameterSheet>(); }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("bNeedCamera", bNeedCamera),                                                                                     \
            MakeNamedPair("bNeedModel", bNeedModel),                                                                                     \
            MakeNamedPair("bNeedResourcePool", bNeedResourcePool),                                                                                     \
            MakeNamedPair("PushConstants", PushConstants)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FShaderParameterSheet_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FShaderParameterSheet::bNeedCamera, "bNeedCamera");                                                                                        \
        Type->RegisterProperty(&FShaderParameterSheet::bNeedModel, "bNeedModel");                                                                                        \
        Type->RegisterProperty(&FShaderParameterSheet::bNeedResourcePool, "bNeedResourcePool");                                                                                        \
        Type->RegisterProperty(&FShaderParameterSheet::PushConstants, "PushConstants");                                                                                        \
    }                                                                                        \
    static inline Z_ShaderParameterSheet_Register Z_REGISTERER_SHADERPARAMETERSHEET;

#define GENERATED_HEADER_FShaderBinaryData                                                                                        \
    struct Z_ShaderBinaryData_Register                                                                                            \
    {                                                                                                                  \
        Z_ShaderBinaryData_Register()                                                                                             \
        {                                                                                                              \
            Register_FShaderBinaryData();                                                                                         \
        }                                                                                                              \
        static void Register_FShaderBinaryData();                                                                                 \
    };                                                                                                                 \
    typedef FShaderBinaryData ThisStruct;                                                                                        \
    static FType GetType() { return TypeOf<FShaderBinaryData>(); }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("ParameterSheet", ParameterSheet),                                                                                     \
            MakeNamedPair("VS", VS),                                                                                     \
            MakeNamedPair("FS", FS)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FShaderBinaryData_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FShaderBinaryData::ParameterSheet, "ParameterSheet");                                                                                        \
        Type->RegisterProperty(&FShaderBinaryData::VS, "VS");                                                                                        \
        Type->RegisterProperty(&FShaderBinaryData::FS, "FS");                                                                                        \
    }                                                                                        \
    static inline Z_ShaderBinaryData_Register Z_REGISTERER_SHADERBINARYDATA;
