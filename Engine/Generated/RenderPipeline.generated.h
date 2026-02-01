#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FRenderPipeline                                                                                        \
    struct Z_RenderPipeline_Register                                                                                            \
    {                                                                                                                  \
        Z_RenderPipeline_Register()                                                                                             \
        {                                                                                                              \
            Register_FRenderPipeline();                                                                                         \
        }                                                                                                              \
        static void Register_FRenderPipeline();                                                                                 \
    };                                                                                                                 \
    typedef FRenderPipeline ThisClass;                                                                                        \
    virtual FType GetType() const { return TypeOf<FRenderPipeline>(); }                                                                                        \
    static constexpr bool IsAbstract() { return true; }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FRenderPipeline)                                                                                        \
    static inline Z_RenderPipeline_Register Z_REGISTERER_RENDERPIPELINE;
