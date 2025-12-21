#pragma once

#include "Core/Utility/Macros.h"

#define GENERATED_HEADER_FRenderConfig                                                                                        \
    struct Z_RenderConfig_Register                                                                                            \
    {                                                                                                                  \
        Z_RenderConfig_Register()                                                                                             \
        {                                                                                                              \
            Register_FRenderConfig();                                                                                         \
        }                                                                                                              \
        static void Register_FRenderConfig();                                                                                 \
    };                                                                                                                 \
    typedef IConfig::ThisClass Super;                                                                                        \
    typedef FRenderConfig ThisClass;                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FRenderConfig)                                                                                        \
    static inline Z_RenderConfig_Register Z_REGISTERER_RENDERCONFIG;
