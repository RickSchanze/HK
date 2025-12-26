#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FAssetImportSetting                                                                                        \
    struct Z_AssetImportSetting_Register                                                                                            \
    {                                                                                                                  \
        Z_AssetImportSetting_Register()                                                                                             \
        {                                                                                                              \
            Register_FAssetImportSetting();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_FAssetImportSetting();                                                                                 \
    };                                                                                                                 \
    typedef FAssetImportSetting ThisClass;                                                                                        \
    virtual FType GetType() const { return TypeOf<FAssetImportSetting>(); }                                                                                        \
    HK_DECL_CLASS_SERIALIZATION(FAssetImportSetting)                                                                                        \
    static inline Z_AssetImportSetting_Register Z_REGISTERER_ASSETIMPORTSETTING;

#define GENERATED_HEADER_FAssetMetaData                                                                                        \
    struct Z_AssetMetaData_Register                                                                                            \
    {                                                                                                                  \
        Z_AssetMetaData_Register()                                                                                             \
        {                                                                                                              \
            Register_FAssetMetaData();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_FAssetMetaData();                                                                                 \
    };                                                                                                                 \
    typedef FAssetMetaData ThisStruct;                                                                                        \
    static FType GetType() { return TypeOf<FAssetMetaData>(); }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("Uuid", Uuid),                                                                                     \
            MakeNamedPair("Path", Path),                                                                                     \
            MakeNamedPair("AssetType", AssetType),                                                                                     \
            MakeNamedPair("FileType", FileType)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FAssetMetaData_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FAssetMetaData::Uuid, "Uuid");                                                                                        \
        Type->RegisterProperty(&FAssetMetaData::Path, "Path");                                                                                        \
        Type->RegisterProperty(&FAssetMetaData::AssetType, "AssetType");                                                                                        \
        Type->RegisterProperty(&FAssetMetaData::FileType, "FileType");                                                                                        \
    }                                                                                        \
    static inline Z_AssetMetaData_Register Z_REGISTERER_ASSETMETADATA;

HK_API void Z_Register_EAssetFileType();
struct F_Z_Register_EAssetFileType
{
    F_Z_Register_EAssetFileType()
    {
        Z_Register_EAssetFileType();
    }
};
static inline F_Z_Register_EAssetFileType Z_REGISTERER_EASSETFILETYPE;
