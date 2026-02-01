#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_CComponent                                                                                        \
    struct Z_Component_Register                                                                                            \
    {                                                                                                                  \
        Z_Component_Register()                                                                                             \
        {                                                                                                              \
            Register_CComponent();                                                                                         \
        }                                                                                                              \
        static void Register_CComponent();                                                                                 \
    };                                                                                                                 \
    typedef HObject::ThisClass Super;                                                                                        \
    typedef CComponent ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<CComponent>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(CComponent)                                                                                        \
    static void Register_CComponent_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&CComponent::bActive, "bActive");                                                                                        \
        Type->RegisterProperty(&CComponent::Owner, "Owner");                                                                                        \
    }                                                                                        \
    static inline Z_Component_Register Z_REGISTERER_COMPONENT;
