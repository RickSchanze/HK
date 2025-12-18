#pragma once

#include "Core/Utility/Macros.h"

#define GENERATED_HEADER_IConfig                                                                                        \
    struct Z_IConfig_Register                                                                                            \
    {                                                                                                                  \
        Z_IConfig_Register()                                                                                             \
        {                                                                                                              \
            Register_IConfig();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_IConfig();                                                                                 \
    };                                                                                                                 \
    typedef IConfig ThisClass;                                                                                        \
    static inline Z_IConfig_Register Z_REGISTERER_ICONFIG;
