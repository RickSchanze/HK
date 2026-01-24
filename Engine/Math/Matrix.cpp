//
// Created by Admin on 2025/12/27.
//

#include "Matrix.h"
#include "Core/Reflection/TypeManager.h"

// FMatrix4x4f 注册实现
static void Register_FMatrix4x4f_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FMatrix4x4f>("Matrix4x4f");
    Type->RegisterProperty(&FMatrix4x4f::M, "M");
}

// FMatrix4x4d 注册实现
static void Register_FMatrix4x4d_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FMatrix4x4d>("Matrix4x4d");
    Type->RegisterProperty(&FMatrix4x4d::M, "M");
}

// FMATRIX_REGISTER::Regsiter 实现
void FMATRIX_REGISTER::Regsiter()
{
    FTypeManager::RegisterTypeRegisterer<FMatrix4x4f>(Register_FMatrix4x4f_Impl);
    FTypeManager::RegisterTypeRegisterer<FMatrix4x4d>(Register_FMatrix4x4d_Impl);
}
