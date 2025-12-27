#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FMeshImportSetting                                                                                        \
    struct Z_MeshImportSetting_Register                                                                                            \
    {                                                                                                                  \
        Z_MeshImportSetting_Register()                                                                                             \
        {                                                                                                              \
            Register_FMeshImportSetting();                                                                                         \
        }                                                                                                              \
        static void Register_FMeshImportSetting();                                                                                 \
    };                                                                                                                 \
    typedef FAssetImportSetting::ThisClass Super;                                                                                        \
    typedef FMeshImportSetting ThisClass;                                                                                        \
    virtual FType GetType() const override { return TypeOf<FMeshImportSetting>(); }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FMeshImportSetting)                                                                                        \
    static void Register_FMeshImportSetting_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FMeshImportSetting::ImportFlags, "ImportFlags");                                                                                        \
    }                                                                                        \
    static inline Z_MeshImportSetting_Register Z_REGISTERER_MESHIMPORTSETTING;

HK_API void Z_Register_EMeshImportFlag();
struct F_Z_Register_EMeshImportFlag
{
    F_Z_Register_EMeshImportFlag()
    {
        Z_Register_EMeshImportFlag();
    }
};
static inline F_Z_Register_EMeshImportFlag Z_REGISTERER_EMESHIMPORTFLAG;
