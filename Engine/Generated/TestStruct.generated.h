#pragma once

#include "Core/Utility/Macros.h"

#define GENERATED_HEADER_FBaseStruct                                                                                        \
    struct Z_BaseStruct_Register                                                                                            \
    {                                                                                                                  \
        Z_BaseStruct_Register()                                                                                             \
        {                                                                                                              \
            Register_FBaseStruct();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_FBaseStruct();                                                                                 \
    };                                                                                                                 \
    typedef FBaseStruct ThisStruct;                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("BaseValue", BaseValue)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static inline Z_BaseStruct_Register Z_REGISTERER_BASESTRUCT;

#define GENERATED_HEADER_FDerivedStruct                                                                                        \
    struct Z_DerivedStruct_Register                                                                                            \
    {                                                                                                                  \
        Z_DerivedStruct_Register()                                                                                             \
        {                                                                                                              \
            Register_FDerivedStruct();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_FDerivedStruct();                                                                                 \
    };                                                                                                                 \
    typedef FBaseStruct::ThisStruct Super;                                                                                        \
    typedef FDerivedStruct ThisStruct;                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Super::Serialize(Ar);                                                                                        \
        Ar(                                                                                                            \
            MakeNamedPair("DerivedValue", DerivedValue)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static inline Z_DerivedStruct_Register Z_REGISTERER_DERIVEDSTRUCT;
