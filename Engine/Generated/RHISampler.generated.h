#pragma once

#include "Core/Utility/Macros.h"

HK_API void Z_Register_ERHIFilter();
struct F_Z_Register_ERHIFilter
{
    F_Z_Register_ERHIFilter()
    {
        Z_Register_ERHIFilter();
    }
};
static inline F_Z_Register_ERHIFilter Z_REGISTERER_ERHIFILTER;
