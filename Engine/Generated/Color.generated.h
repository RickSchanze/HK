#pragma once

#include "Core/Utility/Macros.h"

#define GENERATED_HEADER_FColor                                                                                        \
    struct Z_Color_Register                                                                                            \
    {                                                                                                                  \
        Z_Color_Register()                                                                                             \
        {                                                                                                              \
            Register_FColor();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_FColor();                                                                                 \
    };                                                                                                                 \
    typedef FColor ThisStruct;                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("R", R),                                                                                     \
            MakeNamedPair("G", G),                                                                                     \
            MakeNamedPair("B", B),                                                                                     \
            MakeNamedPair("A", A)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FColor_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FColor::R, "R");                                                                                        \
        Type->RegisterProperty(&FColor::G, "G");                                                                                        \
        Type->RegisterProperty(&FColor::B, "B");                                                                                        \
        Type->RegisterProperty(&FColor::A, "A");                                                                                        \
    }                                                                                        \
    static inline Z_Color_Register Z_REGISTERER_COLOR;
