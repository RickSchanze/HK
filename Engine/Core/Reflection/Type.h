#pragma once

#include "Core/Container/Array.h"
#include "Core/String/Name.h"
#include "ReflectionFwd.h"

struct FTypeImpl
{
    /// 此类型的名字
    FName Name;
    /// 此类型自己定义的属性
    TArray<FProperty> Properties;
    /// 此类型所有的父类
    TArray<FType> Bases;
    /// 此类型所有的方法
    TArray<FMethod> Methods;
    /// 此类型的注解
    FAttributeMap Attributes;

    TArray<FProperty> GetAllProperties() const;
    TArray<FMethod> GetAllMethods() const;

    bool IsSubclassOf(FType InBaseType) const;
    bool IsDerivedFrom(FType InBaseType) const;

    bool HasAttribute(FName InAttributeName) const;
    FName GetAttribute(FName InAttributeName) const;
};
