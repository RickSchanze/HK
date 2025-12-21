#pragma once

#include "Core/Utility/Macros.h"

#define GENERATED_HEADER_FRHIConfig                                                                                        \
    struct Z_RHIConfig_Register                                                                                            \
    {                                                                                                                  \
        Z_RHIConfig_Register()                                                                                             \
        {                                                                                                              \
            Register_FRHIConfig();                                                                                         \
        }                                                                                                              \
        static void Register_FRHIConfig();                                                                                 \
    };                                                                                                                 \
    typedef IConfig::ThisClass Super;                                                                                        \
    typedef FRHIConfig ThisClass;                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FRHIConfig)                                                                                        \
    static void Register_FRHIConfig_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FRHIConfig::DefaultWindowSize, "DefaultWindowSize");                                                                                        \
        Type->RegisterProperty(&FRHIConfig::GfxBackend, "GfxBackend");                                                                                        \
    }                                                                                        \
    const FVector2i& GetDefaultWindowSize() const { return DefaultWindowSize; }                                                                                        \
    void SetDefaultWindowSize(const FVector2i& InValue) { DefaultWindowSize = InValue; }                                                                                        \
    const EGfxBackend& GetGfxBackend() const { return GfxBackend; }                                                                                        \
    void SetGfxBackend(const EGfxBackend& InValue) { GfxBackend = InValue; }                                                                                        \
    static inline Z_RHIConfig_Register Z_REGISTERER_RHICONFIG;
