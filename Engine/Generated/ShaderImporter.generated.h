#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FShaderImportSetting                                                                                        \
    struct Z_ShaderImportSetting_Register                                                                                            \
    {                                                                                                                  \
        Z_ShaderImportSetting_Register()                                                                                             \
        {                                                                                                              \
            Register_FShaderImportSetting();                                                                                         \
        }                                                                                                              \
        static void Register_FShaderImportSetting();                                                                                 \
    };                                                                                                                 \
    typedef FAssetImportSetting::ThisClass Super;                                                                                        \
    typedef FShaderImportSetting ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<FShaderImportSetting>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FShaderImportSetting)                                                                                        \
    static inline Z_ShaderImportSetting_Register Z_REGISTERER_SHADERIMPORTSETTING;

#define GENERATED_HEADER_FShaderIntermediate                                                                                        \
    struct Z_ShaderIntermediate_Register                                                                                            \
    {                                                                                                                  \
        Z_ShaderIntermediate_Register()                                                                                             \
        {                                                                                                              \
            Register_FShaderIntermediate();                                                                                         \
        }                                                                                                              \
        static void Register_FShaderIntermediate();                                                                                 \
    };                                                                                                                 \
    typedef FShaderIntermediate ThisStruct;                                                                                        \
    static FType GetType() { return TypeOf<FShaderIntermediate>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("Hash", Hash),                                                                                     \
            MakeNamedPair("BinaryData", BinaryData)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FShaderIntermediate_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FShaderIntermediate::Hash, "Hash");                                                                                        \
        Type->RegisterProperty(&FShaderIntermediate::BinaryData, "BinaryData");                                                                                        \
    }                                                                                        \
    static inline Z_ShaderIntermediate_Register Z_REGISTERER_SHADERINTERMEDIATE;
