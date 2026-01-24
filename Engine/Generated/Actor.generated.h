#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_AActor                                                                                        \
    struct Z_AActor_Register                                                                                            \
    {                                                                                                                  \
        Z_AActor_Register()                                                                                             \
        {                                                                                                              \
            Register_AActor();                                                                                         \
        }                                                                                                              \
        static void Register_AActor();                                                                                 \
    };                                                                                                                 \
    typedef HObject::ThisClass Super;                                                                                        \
    typedef AActor ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<AActor>(); }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(AActor)                                                                                        \
    static void Register_AActor_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&AActor::Components, "Components");                                                                                        \
        Type->RegisterProperty(&AActor::RelativeTransform, "RelativeTransform");                                                                                        \
    }                                                                                        \
    static inline Z_AActor_Register Z_REGISTERER_AACTOR;
