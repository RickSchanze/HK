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
    static inline Z_Color_Register Z_REGISTERER_COLOR;

HK_API void Z_Register_MyEnum();
struct F_Z_Register_MyEnum
{
    F_Z_Register_MyEnum()
    {
        Z_Register_MyEnum();
    }
};
static inline F_Z_Register_MyEnum Z_REGISTERER_MYENUM;
