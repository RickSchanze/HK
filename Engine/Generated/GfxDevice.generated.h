#pragma once

#include "Core/Utility/Macros.h"

HK_API void Z_Register_EGfxBackend();
struct F_Z_Register_EGfxBackend
{
    F_Z_Register_EGfxBackend()
    {
        Z_Register_EGfxBackend();
    }
};
static inline F_Z_Register_EGfxBackend Z_REGISTERER_EGFXBACKEND;
