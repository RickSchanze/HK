#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FEngineConfig                                                                                        \
    struct Z_EngineConfig_Register                                                                                            \
    {                                                                                                                  \
        Z_EngineConfig_Register()                                                                                             \
        {                                                                                                              \
            Register_FEngineConfig();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_FEngineConfig();                                                                                 \
    };                                                                                                                 \
    typedef IConfig::ThisClass Super;                                                                                        \
    typedef FEngineConfig ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<FEngineConfig>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FEngineConfig)                                                                                        \
    static void Register_FEngineConfig_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FEngineConfig::DefaultObjectCount, "DefaultObjectCount");                                                                                        \
        Type->RegisterProperty(&FEngineConfig::DefaultObjectIncreaseCount, "DefaultObjectIncreaseCount");                                                                                        \
    }                                                                                        \
    Int32 GetDefaultObjectCount() const { return DefaultObjectCount; }                                                                                        \
    void SetDefaultObjectCount(Int32 InValue) { DefaultObjectCount = InValue; }                                                                                        \
    Int32 GetDefaultObjectIncreaseCount() const { return DefaultObjectIncreaseCount; }                                                                                        \
    void SetDefaultObjectIncreaseCount(Int32 InValue) { DefaultObjectIncreaseCount = InValue; }                                                                                        \
    static inline Z_EngineConfig_Register Z_REGISTERER_ENGINECONFIG;
