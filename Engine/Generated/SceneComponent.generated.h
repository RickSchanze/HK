#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_CSceneComponent                                                                                        \
    struct Z_SceneComponent_Register                                                                                            \
    {                                                                                                                  \
        Z_SceneComponent_Register()                                                                                             \
        {                                                                                                              \
            Register_CSceneComponent();                                                                                         \
        }                                                                                                              \
        static void Register_CSceneComponent();                                                                                 \
    };                                                                                                                 \
    typedef CComponent::ThisClass Super;                                                                                        \
    typedef CSceneComponent ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<CSceneComponent>(); }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(CSceneComponent)                                                                                        \
    static void Register_CSceneComponent_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&CSceneComponent::LocalTransform, "LocalTransform");                                                                                        \
    }                                                                                        \
    static inline Z_SceneComponent_Register Z_REGISTERER_SCENECOMPONENT;
