#pragma once

#include "Core/Utility/Macros.h"

#include "Core/Reflection/TypeManager.h"

#define GENERATED_HEADER_FVertexPNU                                                                                        \
    struct Z_VertexPNU_Register                                                                                            \
    {                                                                                                                  \
        Z_VertexPNU_Register()                                                                                             \
        {                                                                                                              \
            Register_FVertexPNU();                                                                                         \
        }                                                                                                              \
        static void Register_FVertexPNU();                                                                                 \
    };                                                                                                                 \
    typedef FVertexPNU ThisStruct;                                                                                        \
    static FType GetType() { return TypeOf<FVertexPNU>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("Position", Position),                                                                                     \
            MakeNamedPair("Normal", Normal),                                                                                     \
            MakeNamedPair("UV", UV)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FVertexPNU_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FVertexPNU::Position, "Position");                                                                                        \
        Type->RegisterProperty(&FVertexPNU::Normal, "Normal");                                                                                        \
        Type->RegisterProperty(&FVertexPNU::UV, "UV");                                                                                        \
    }                                                                                        \
    static inline Z_VertexPNU_Register Z_REGISTERER_VERTEXPNU;

#define GENERATED_HEADER_FSubMeshIntermediate                                                                                        \
    struct Z_SubMeshIntermediate_Register                                                                                            \
    {                                                                                                                  \
        Z_SubMeshIntermediate_Register()                                                                                             \
        {                                                                                                              \
            Register_FSubMeshIntermediate();                                                                                         \
        }                                                                                                              \
        static void Register_FSubMeshIntermediate();                                                                                 \
    };                                                                                                                 \
    typedef FSubMeshIntermediate ThisStruct;                                                                                        \
    static FType GetType() { return TypeOf<FSubMeshIntermediate>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("Vertices", Vertices),                                                                                     \
            MakeNamedPair("Indices", Indices)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FSubMeshIntermediate_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FSubMeshIntermediate::Vertices, "Vertices");                                                                                        \
        Type->RegisterProperty(&FSubMeshIntermediate::Indices, "Indices");                                                                                        \
    }                                                                                        \
    static inline Z_SubMeshIntermediate_Register Z_REGISTERER_SUBMESHINTERMEDIATE;

#define GENERATED_HEADER_FMeshIntermediate                                                                                        \
    struct Z_MeshIntermediate_Register                                                                                            \
    {                                                                                                                  \
        Z_MeshIntermediate_Register()                                                                                             \
        {                                                                                                              \
            Register_FMeshIntermediate();                                                                                         \
        }                                                                                                              \
        static void Register_FMeshIntermediate();                                                                                 \
    };                                                                                                                 \
    typedef FMeshIntermediate ThisStruct;                                                                                        \
    static FType GetType() { return TypeOf<FMeshIntermediate>(); }                                                                                        \
    static constexpr bool IsAbstract() { return false; }                                                                                        \
    template <typename Archive>                                                                                        \
    void Serialize(Archive& Ar)                                                                                        \
    {                                                                                                                  \
        Ar(                                                                                                            \
            MakeNamedPair("Hash", Hash),                                                                                     \
            MakeNamedPair("SubMeshes", SubMeshes)                                                                                      \
        );                                                                                                             \
    }                                                                                                                  \
    static void Register_FMeshIntermediate_Properties(FTypeMutable Type)                                                                                        \
    {                                                                                        \
        Type->RegisterProperty(&FMeshIntermediate::Hash, "Hash");                                                                                        \
        Type->RegisterProperty(&FMeshIntermediate::SubMeshes, "SubMeshes");                                                                                        \
    }                                                                                        \
    static inline Z_MeshIntermediate_Register Z_REGISTERER_MESHINTERMEDIATE;

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
    static constexpr bool IsAbstract() { return false; }                                                                                        \
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
