#pragma once

#include "Core/Container/Array.h"
#include "Core/String/Name.h"
#include "Core/Utility/Macros.h"
#include "ReflectionFwd.h"

// 前向声明
struct FPropertyImpl;

struct FTypeImpl
{
    /**
      * 此类型的名字
      */
    FName Name;
    /**
      * 此类型自己定义的属性
      */
    TArray<FProperty> Properties;
    /**
      * 此类型所有的父类
      */
    TArray<FType> Bases;
    /**
      * 此类型所有的方法
      */
    TArray<FMethod> Methods;
    /**
      * 此类型的注解
      */
    FAttributeMap Attributes;
    /**
      * 类型标志
      */
    ETypeFlags Flags;
    /**
      * 类型大小（字节）
      */
    Int32 Size;
    /**
      * 如果是Enum，底层类型
      */
    FType UnderlyingType;

    FTypeImpl() : Flags(ETypeFlags::None), Size(0), UnderlyingType(nullptr) {}

    TArray<FProperty> GetAllProperties() const;
    TArray<FMethod> GetAllMethods() const;

    bool IsSubclassOf(FType InBaseType) const;
    bool IsDerivedFrom(FType InBaseType) const;

    /**
      * 检查是否是Enum类型
      */
    bool IsEnum() const
    {
        return (Flags & ETypeFlags::Enum) != ETypeFlags::None;
    }

    /**
      * 获取Enum的底层类型
      */
    FType GetUnderlyingType() const
    {
        return UnderlyingType;
    }

    /**
      * 注册属性（返回TypeMutable*支持链式调用）
      * 注意：PropertyType必须在注册Property之前已经注册
      */
    template <typename ClassType, typename PropertyType>
    FTypeMutable RegisterProperty(PropertyType ClassType::* InMemberPtr, const char* InName);

    /**
      * 注册枚举成员（返回TypeMutable*支持链式调用）
      * EnumValue作为Offset存储，Name为枚举成员名字
      */
    template <typename EnumType>
    FTypeMutable RegisterEnumMember(EnumType InEnumValue, const char* InName);

    /**
      * 注册父类（返回Type*支持链式调用）
      */
    FTypeMutable RegisterParent(FType InParentType);

    /**
      * 注册类型注解（链式调用）
      */
    FTypeMutable RegisterAttribute(FName InAttributeName, FName InAttributeValue);

    /**
      * 检查是否有注解
      */
    bool HasAttribute(const FName InAttributeName) const
    {
        return Attributes.Contains(InAttributeName);
    }

    /**
      * 获取注解
      */
    FStringView GetAttribute(const FName InAttributeName) const
    {
        const FString* Value = Attributes.Find(InAttributeName);
        return Value != nullptr ? *Value : "";
    }

    /**
      * 检查是否是Abstract类型
      */
    bool IsAbstract() const
    {
        return HasAttribute(FName("Abstract"));
    }

    /**
      * 检查是否是Interface类型
      */
    bool IsInterface() const
    {
        return HasAttribute(FName("Interface"));
    }

    /**
      * 检查是否可以创建实例（不是Abstract也不是Interface）
      */
    bool CanCreateInstance() const
    {
        return !IsAbstract() && !IsInterface();
    }

    /**
      * 创建实例（返回void*，需要外部管理内存）
      */
    void* CreateInstance() const;

    /**
      * 销毁实例
      */
    void DestroyInstance(void* InInstance) const;

    /**
      * 创建实例（模板版本，返回T*）
      */
    template <typename T>
    T* CreateInstanceT() const;

    /**
      * 创建unique_ptr实例
      */
    template <typename T>
    std::unique_ptr<T> CreateUnique() const;

    /**
      * 创建shared_ptr实例
      */
    template <typename T>
    std::shared_ptr<T> CreateShared() const;
};

// 包含内联实现
#include "Type.inl"
