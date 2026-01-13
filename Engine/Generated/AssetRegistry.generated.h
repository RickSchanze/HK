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

#define GENERATED_HEADER_FAssetMetadata                                                                                        \
    struct Z_AssetMetadata_Register                                                                                            \
    {                                                                                                                  \
        Z_AssetMetadata_Register()                                                                                             \
        {                                                                                                              \
            Register_FAssetMetadata();                                                                                         \
        }                                                                                                              \
        static HK_API void Register_FAssetMetadata();                                                                                 \
    };                                                                                                                 \
    typedef FAssetMetadata ThisStruct;                                                                                        \
    static FType GetType() { return TypeOf<FAssetMetadata>(); }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("Uuid", Uuid),                                                                                     \
            MakeNamedPair("Path", Path),                                                                                     \
            MakeNamedPair("AssetType", AssetType),                                                                                     \
            MakeNamedPair("FileType", FileType),                                                                                     \
            MakeNamedPair("ImportSetting", ImportSetting),                                                                                     \
            MakeNamedPair("IntermediateHash", IntermediateHash)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FAssetMetadata_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FAssetMetadata::Uuid, "Uuid");                                                                                        \
        Type->RegisterProperty(&FAssetMetadata::Path, "Path");                                                                                        \
        Type->RegisterProperty(&FAssetMetadata::AssetType, "AssetType");                                                                                        \
        Type->RegisterProperty(&FAssetMetadata::FileType, "FileType");                                                                                        \
        Type->RegisterProperty(&FAssetMetadata::ImportSetting, "ImportSetting");                                                                                        \
        Type->RegisterProperty(&FAssetMetadata::IntermediateHash, "IntermediateHash");                                                                                        \
    }                                                                                        \
    static inline Z_AssetMetadata_Register Z_REGISTERER_ASSETMETADATA;

HK_API void Z_Register_EAssetFileType();
struct F_Z_Register_EAssetFileType
{
    F_Z_Register_EAssetFileType()
    {
        Z_Register_EAssetFileType();
    }
};
static inline F_Z_Register_EAssetFileType Z_REGISTERER_EASSETFILETYPE;
