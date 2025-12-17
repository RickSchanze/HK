//
// Created by Admin on 2025/12/16.
//

#include "Vector.h"
#include "Core/Reflection/TypeManager.h"

// FVector2f 注册实现
static void Register_FVector2f_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector2f>("Vector2f");
    Type->RegisterProperty(&FVector2f::X, "X");
    Type->RegisterProperty(&FVector2f::Y, "Y");
}

// FVector2d 注册实现
static void Register_FVector2d_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector2d>("Vector2d");
    Type->RegisterProperty(&FVector2d::X, "X");
    Type->RegisterProperty(&FVector2d::Y, "Y");
}

// FVector2i 注册实现
static void Register_FVector2i_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector2i>("Vector2i");
    Type->RegisterProperty(&FVector2i::X, "X");
    Type->RegisterProperty(&FVector2i::Y, "Y");
}

// FVector2u 注册实现
static void Register_FVector2u_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector2u>("Vector2u");
    Type->RegisterProperty(&FVector2u::X, "X");
    Type->RegisterProperty(&FVector2u::Y, "Y");
}

// FVector3f 注册实现
static void Register_FVector3f_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector3f>("Vector3f");
    Type->RegisterProperty(&FVector3f::X, "X");
    Type->RegisterProperty(&FVector3f::Y, "Y");
    Type->RegisterProperty(&FVector3f::Z, "Z");
}

// FVector3d 注册实现
static void Register_FVector3d_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector3d>("Vector3d");
    Type->RegisterProperty(&FVector3d::X, "X");
    Type->RegisterProperty(&FVector3d::Y, "Y");
    Type->RegisterProperty(&FVector3d::Z, "Z");
}

// FVector3i 注册实现
static void Register_FVector3i_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector3i>("Vector3i");
    Type->RegisterProperty(&FVector3i::X, "X");
    Type->RegisterProperty(&FVector3i::Y, "Y");
    Type->RegisterProperty(&FVector3i::Z, "Z");
}

// FVector3u 注册实现
static void Register_FVector3u_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector3u>("Vector3u");
    Type->RegisterProperty(&FVector3u::X, "X");
    Type->RegisterProperty(&FVector3u::Y, "Y");
    Type->RegisterProperty(&FVector3u::Z, "Z");
}

// FVector4f 注册实现
static void Register_FVector4f_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector4f>("Vector4f");
    Type->RegisterProperty(&FVector4f::X, "X");
    Type->RegisterProperty(&FVector4f::Y, "Y");
    Type->RegisterProperty(&FVector4f::Z, "Z");
    Type->RegisterProperty(&FVector4f::W, "W");
}

// FVector4d 注册实现
static void Register_FVector4d_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector4d>("Vector4d");
    Type->RegisterProperty(&FVector4d::X, "X");
    Type->RegisterProperty(&FVector4d::Y, "Y");
    Type->RegisterProperty(&FVector4d::Z, "Z");
    Type->RegisterProperty(&FVector4d::W, "W");
}

// FVector4i 注册实现
static void Register_FVector4i_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector4i>("Vector4i");
    Type->RegisterProperty(&FVector4i::X, "X");
    Type->RegisterProperty(&FVector4i::Y, "Y");
    Type->RegisterProperty(&FVector4i::Z, "Z");
    Type->RegisterProperty(&FVector4i::W, "W");
}

// FVector4u 注册实现
static void Register_FVector4u_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FVector4u>("Vector4u");
    Type->RegisterProperty(&FVector4u::X, "X");
    Type->RegisterProperty(&FVector4u::Y, "Y");
    Type->RegisterProperty(&FVector4u::Z, "Z");
    Type->RegisterProperty(&FVector4u::W, "W");
}

// FVECTOR2_REGISTER::Regsiter 实现
void FVECTOR2_REGISTER::Regsiter()
{
    FTypeManager::RegisterTypeRegisterer<FVector2f>(Register_FVector2f_Impl);
    FTypeManager::RegisterTypeRegisterer<FVector2d>(Register_FVector2d_Impl);
    FTypeManager::RegisterTypeRegisterer<FVector2i>(Register_FVector2i_Impl);
    FTypeManager::RegisterTypeRegisterer<FVector2u>(Register_FVector2u_Impl);
}

// FVECTOR3_REGISTER::Regsiter 实现
void FVECTOR3_REGISTER::Regsiter()
{
    FTypeManager::RegisterTypeRegisterer<FVector3f>(Register_FVector3f_Impl);
    FTypeManager::RegisterTypeRegisterer<FVector3d>(Register_FVector3d_Impl);
    FTypeManager::RegisterTypeRegisterer<FVector3i>(Register_FVector3i_Impl);
    FTypeManager::RegisterTypeRegisterer<FVector3u>(Register_FVector3u_Impl);
}

// FVECTOR4_REGISTER::Regsiter 实现
void FVECTOR4_REGISTER::Regsiter()
{
    FTypeManager::RegisterTypeRegisterer<FVector4f>(Register_FVector4f_Impl);
    FTypeManager::RegisterTypeRegisterer<FVector4d>(Register_FVector4d_Impl);
    FTypeManager::RegisterTypeRegisterer<FVector4i>(Register_FVector4i_Impl);
    FTypeManager::RegisterTypeRegisterer<FVector4u>(Register_FVector4u_Impl);
}
