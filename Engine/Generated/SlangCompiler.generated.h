#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FShaderCompileResult                                                                                        \
    struct Z_ShaderCompileResult_Register                                                                                            \
    {                                                                                                                  \
        Z_ShaderCompileResult_Register()                                                                                             \
        {                                                                                                              \
            Register_FShaderCompileResult();                                                                                         \
        }                                                                                                              \
        static void Register_FShaderCompileResult();                                                                                 \
    };                                                                                                                 \
    typedef FShaderCompileResult ThisStruct;                                                                                        \
    static FType GetType() { return TypeOf<FShaderCompileResult>(); }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("ParameterSheet", ParameterSheet),                                                                                     \
            MakeNamedPair("VS", VS),                                                                                     \
            MakeNamedPair("FS", FS)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FShaderCompileResult_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FShaderCompileResult::ParameterSheet, "ParameterSheet");                                                                                        \
        Type->RegisterProperty(&FShaderCompileResult::VS, "VS");                                                                                        \
        Type->RegisterProperty(&FShaderCompileResult::FS, "FS");                                                                                        \
    }                                                                                        \
    static inline Z_ShaderCompileResult_Register Z_REGISTERER_SHADERCOMPILERESULT;
