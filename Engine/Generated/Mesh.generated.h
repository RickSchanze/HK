#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_HMesh                                                                                        \
    struct Z_HMesh_Register                                                                                            \
    {                                                                                                                  \
        Z_HMesh_Register()                                                                                             \
        {                                                                                                              \
            Register_HMesh();                                                                                         \
        }                                                                                                              \
        static void Register_HMesh();                                                                                 \
    };                                                                                                                 \
    typedef HAsset::ThisClass Super;                                                                                        \
    typedef HMesh ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<HMesh>(); }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(HMesh)                                                                                        \
    static inline Z_HMesh_Register Z_REGISTERER_HMESH;
