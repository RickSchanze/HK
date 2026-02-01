#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FHKRenderPipeline                                                                                        \
    struct Z_HKRenderPipeline_Register                                                                                            \
    {                                                                                                                  \
        Z_HKRenderPipeline_Register()                                                                                             \
        {                                                                                                              \
            Register_FHKRenderPipeline();                                                                                         \
        }                                                                                                              \
        static void Register_FHKRenderPipeline();                                                                                 \
    };                                                                                                                 \
    typedef FRenderPipeline::ThisClass Super;                                                                                        \
    typedef FHKRenderPipeline ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<FHKRenderPipeline>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FHKRenderPipeline)                                                                                        \
    static inline Z_HKRenderPipeline_Register Z_REGISTERER_HKRENDERPIPELINE;
