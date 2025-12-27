#pragma once

#include "Core/Utility/Macros.h"

HK_API void Z_Register_EAssetImportOptions();
struct F_Z_Register_EAssetImportOptions
{
    F_Z_Register_EAssetImportOptions()
    {
        Z_Register_EAssetImportOptions();
    }
};
static inline F_Z_Register_EAssetImportOptions Z_REGISTERER_EASSETIMPORTOPTIONS;
