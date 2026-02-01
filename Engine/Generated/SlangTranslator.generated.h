#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FShaderTranslateResult                                                                                        \
    struct Z_ShaderTranslateResult_Register                                                                                            \
    {                                                                                                                  \
        Z_ShaderTranslateResult_Register()                                                                                             \
        {                                                                                                              \
            Register_FShaderTranslateResult();                                                                                         \
        }                                                                                                              \
        static void Register_FShaderTranslateResult();                                                                                 \
    };                                                                                                                 \
    typedef FShaderTranslateResult ThisStruct;                                                                                        \
    static FType GetType() { return TypeOf<FShaderTranslateResult>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("ParameterSheet", ParameterSheet),                                                                                     \
            MakeNamedPair("VS", VS),                                                                                     \
            MakeNamedPair("FS", FS)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FShaderTranslateResult_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FShaderTranslateResult::ParameterSheet, "ParameterSheet");                                                                                        \
        Type->RegisterProperty(&FShaderTranslateResult::VS, "VS");                                                                                        \
        Type->RegisterProperty(&FShaderTranslateResult::FS, "FS");                                                                                        \
    }                                                                                        \
    static inline Z_ShaderTranslateResult_Register Z_REGISTERER_SHADERTRANSLATERESULT;
