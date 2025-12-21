#pragma once

#include "Core/Utility/Macros.h"

HK_API void Z_Register_ERHIImageFormat();
struct F_Z_Register_ERHIImageFormat
{
    F_Z_Register_ERHIImageFormat()
    {
        Z_Register_ERHIImageFormat();
    }
};
static inline F_Z_Register_ERHIImageFormat Z_REGISTERER_ERHIIMAGEFORMAT;
