#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

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
    virtual FType GetType() const override { return TypeOf<FRenderConfig>(); }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FRenderConfig)                                                                                        \
    static void Register_FRenderConfig_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FRenderConfig::CommitStyle, "CommitStyle");                                                                                        \
    }                                                                                        \
    const ERenderCommandCommitStyle& GetCommitStyle() const { return CommitStyle; }                                                                                        \
    void SetCommitStyle(const ERenderCommandCommitStyle& InValue) { CommitStyle = InValue; }                                                                                        \
    static inline Z_RenderConfig_Register Z_REGISTERER_RENDERCONFIG;

HK_API void Z_Register_ERenderCommandCommitStyle();
struct F_Z_Register_ERenderCommandCommitStyle
{
    F_Z_Register_ERenderCommandCommitStyle()
    {
        Z_Register_ERenderCommandCommitStyle();
    }
};
static inline F_Z_Register_ERenderCommandCommitStyle Z_REGISTERER_ERENDERCOMMANDCOMMITSTYLE;
