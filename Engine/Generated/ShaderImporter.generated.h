#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FShaderImportSetting                                                                                        \
    struct Z_ShaderImportSetting_Register                                                                                            \
    {                                                                                                                  \
        Z_ShaderImportSetting_Register()                                                                                             \
        {                                                                                                              \
            Register_FShaderImportSetting();                                                                                         \
        }                                                                                                              \
        static void Register_FShaderImportSetting();                                                                                 \
    };                                                                                                                 \
    typedef FAssetImportSetting::ThisClass Super;                                                                                        \
    typedef FShaderImportSetting ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<FShaderImportSetting>(); }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FShaderImportSetting)                                                                                        \
    static inline Z_ShaderImportSetting_Register Z_REGISTERER_SHADERIMPORTSETTING;
