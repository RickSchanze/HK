#pragma once

#include "Core/Utility/Macros.h"

#define GENERATED_HEADER_FBaseClass                                                                                        \
    struct Z_BaseClass_Register                                                                                            \
    {                                                                                                                  \
        Z_BaseClass_Register()                                                                                             \
        {                                                                                                              \
            Register_FBaseClass();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_FBaseClass();                                                                                 \
    };                                                                                                                 \
    typedef FBaseClass ThisClass;                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FBaseClass)                                                                                        \
    static inline Z_BaseClass_Register Z_REGISTERER_BASECLASS;

#define GENERATED_HEADER_FDerivedClass                                                                                        \
    struct Z_DerivedClass_Register                                                                                            \
    {                                                                                                                  \
        Z_DerivedClass_Register()                                                                                             \
        {                                                                                                              \
            Register_FDerivedClass();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_FDerivedClass();                                                                                 \
    };                                                                                                                 \
    typedef FBaseClass::ThisClass Super;                                                                                        \
    typedef FDerivedClass ThisClass;                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FDerivedClass)                                                                                        \
    static inline Z_DerivedClass_Register Z_REGISTERER_DERIVEDCLASS;

#define GENERATED_HEADER_FSimpleClass                                                                                        \
    struct Z_SimpleClass_Register                                                                                            \
    {                                                                                                                  \
        Z_SimpleClass_Register()                                                                                             \
        {                                                                                                              \
            Register_FSimpleClass();                                                                                         \
        }                                                                                                              \
        static void Register_FSimpleClass();                                                                                 \
    };                                                                                                                 \
    typedef FSimpleClass ThisClass;                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FSimpleClass)                                                                                        \
    static inline Z_SimpleClass_Register Z_REGISTERER_SIMPLECLASS;
