//
// Created by Admin on 2025/12/27.
//

#include "Transform.h"
#include "Core/Reflection/TypeManager.h"

// FTransformf 注册实现
static void Register_FTransformf_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FTransformf>("Transformf");
    Type->RegisterProperty(&FTransformf::Position, "Position");
    Type->RegisterProperty(&FTransformf::Rotation, "Rotation");
    Type->RegisterProperty(&FTransformf::Scale, "Scale");
}

// FTransformd 注册实现
static void Register_FTransformd_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FTransformd>("Transformd");
    Type->RegisterProperty(&FTransformd::Position, "Position");
    Type->RegisterProperty(&FTransformd::Rotation, "Rotation");
    Type->RegisterProperty(&FTransformd::Scale, "Scale");
}

// FTRANSFORM_REGISTER::Regsiter 实现
void FTRANSFORM_REGISTER::Regsiter()
{
    FTypeManager::RegisterTypeRegisterer<FTransformf>(Register_FTransformf_Impl);
    FTypeManager::RegisterTypeRegisterer<FTransformd>(Register_FTransformd_Impl);
}
