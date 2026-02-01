#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_CRendererComponent                                                                                        \
    struct Z_RendererComponent_Register                                                                                            \
    {                                                                                                                  \
        Z_RendererComponent_Register()                                                                                             \
        {                                                                                                              \
            Register_CRendererComponent();                                                                                         \
        }                                                                                                              \
        static void Register_CRendererComponent();                                                                                 \
    };                                                                                                                 \
    typedef CSceneComponent::ThisClass Super;                                                                                        \
    typedef CRendererComponent ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<CRendererComponent>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(CRendererComponent)                                                                                        \
    static inline Z_RendererComponent_Register Z_REGISTERER_RENDERERCOMPONENT;
