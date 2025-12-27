//
// Created by Admin on 2025/12/27.
//

#include "Rect2D.h"
#include "Core/Reflection/TypeManager.h"

// FRect2Di 注册实现
static void Register_FRect2Di_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FRect2Di>("Rect2Di");
    Type->RegisterProperty(&FRect2Di::X, "X");
    Type->RegisterProperty(&FRect2Di::Y, "Y");
    Type->RegisterProperty(&FRect2Di::Width, "Width");
    Type->RegisterProperty(&FRect2Di::Height, "Height");
}

// FRect2Df 注册实现
static void Register_FRect2Df_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FRect2Df>("Rect2Df");
    Type->RegisterProperty(&FRect2Df::X, "X");
    Type->RegisterProperty(&FRect2Df::Y, "Y");
    Type->RegisterProperty(&FRect2Df::Width, "Width");
    Type->RegisterProperty(&FRect2Df::Height, "Height");
}

// FRect2Dd 注册实现
static void Register_FRect2Dd_Impl()
{
    FTypeMutable Type = FTypeManager::Register<FRect2Dd>("Rect2Dd");
    Type->RegisterProperty(&FRect2Dd::X, "X");
    Type->RegisterProperty(&FRect2Dd::Y, "Y");
    Type->RegisterProperty(&FRect2Dd::Width, "Width");
    Type->RegisterProperty(&FRect2Dd::Height, "Height");
}

// FRECT2D_REGISTER::Regsiter 实现
void FRECT2D_REGISTER::Regsiter()
{
    FTypeManager::RegisterTypeRegisterer<FRect2Di>(Register_FRect2Di_Impl);
    FTypeManager::RegisterTypeRegisterer<FRect2Df>(Register_FRect2Df_Impl);
    FTypeManager::RegisterTypeRegisterer<FRect2Dd>(Register_FRect2Dd_Impl);
}

