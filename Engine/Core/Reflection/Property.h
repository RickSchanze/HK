#pragma once

#include "Core/String/Name.h"
#include "Core/Utility/Macros.h"
#include "Core/Utility/Optional.h"
#include "ReflectionFwd.h"

// 前向声明
template <typename T>
FType TypeOf();

struct FPropertyImpl
{
    /// 属性名称
    FName Name;
    /// 属性类型（对于Array/FixedArray，也指代ElementType；对于Map，也指代ValueType）
    FType Type;
    /// 属性标志
    EPropertyFlags Flags;
    /// 属性在对象中的偏移量（字节），对于枚举成员，这是EnumValue
    Int32 Offset;
    /// 声明此属性的类型
    FType OwnerType;
    /// 如果是Map，Key类型（ValueType使用Type）
    FType KeyType;
    /// 此属性的注解
    FAttributeMap Attributes;

    FPropertyImpl()
        : Name(), Type(nullptr), Flags(EPropertyFlags::None), Offset(0), OwnerType(nullptr), KeyType(nullptr)
    {
    }

    /// 检查是否是Enum字段
    bool IsEnum() const
    {
        return (Flags & EPropertyFlags::Enum) != EPropertyFlags::None;
    }

    /// 检查是否是Array
    bool IsArray() const
    {
        return (Flags & EPropertyFlags::Array) != EPropertyFlags::None;
    }

    /// 检查是否是FixedArray
    bool IsFixedArray() const
    {
        return (Flags & EPropertyFlags::FixedArray) != EPropertyFlags::None;
    }

    /// 检查是否是Map
    bool IsMap() const
    {
        return (Flags & EPropertyFlags::Map) != EPropertyFlags::None;
    }

    /// 获取ElementType（Array/FixedArray的元素类型，使用Type）
    FType GetElementType() const
    {
        if (IsArray() || IsFixedArray())
        {
            return Type;
        }
        return nullptr;
    }

    /// 获取KeyType（Map的Key类型）
    FType GetKeyType() const
    {
        if (IsMap())
        {
            return KeyType;
        }
        return nullptr;
    }

    /// 获取ValueType（Map的Value类型，使用Type）
    FType GetValueType() const
    {
        if (IsMap())
        {
            return Type;
        }
        return nullptr;
    }

    /// 获取枚举成员的值（如果是枚举成员）
    template <typename EnumType>
    TOptional<EnumType> GetEnumPropertyValue() const
    {
        if (!IsEnum())
        {
            return TOptional<EnumType>();
        }
        return TOptional<EnumType>(static_cast<EnumType>(Offset));
    }

    /// 获取枚举成员的值（返回Int32）
    Int32 GetEnumValue() const
    {
        if (!IsEnum())
        {
            return 0;
        }
        return Offset;
    }

    /// 获取属性在对象中的地址
    template <typename ObjectType>
    void* GetAddress(ObjectType* InObject) const
    {
        return reinterpret_cast<char*>(InObject) + Offset;
    }

    /// 获取属性在对象中的地址（const版本）
    template <typename ObjectType>
    const void* GetAddress(const ObjectType* InObject) const
    {
        return reinterpret_cast<const char*>(InObject) + Offset;
    }

    /// 获取属性值（成员函数）
    template <typename T, typename ObjectType>
    TOptional<T> GetValue(ObjectType* InObject) const
    {
        if (InObject == nullptr)
        {
            return TOptional<T>();
        }
        void* Addr = GetAddress(InObject);
        if (Addr == nullptr)
        {
            return TOptional<T>();
        }
        // 类型检查 - 需要包含TypeManager.h才能使用TypeOf
        // 这里先不做类型检查，直接返回
        return TOptional<T>(*reinterpret_cast<T*>(Addr));
    }

    /// 获取属性值（const版本）
    template <typename T, typename ObjectType>
    TOptional<T> GetValue(const ObjectType* InObject) const
    {
        if (InObject == nullptr)
        {
            return TOptional<T>();
        }
        const void* Addr = GetAddress(InObject);
        if (Addr == nullptr)
        {
            return TOptional<T>();
        }
        // 类型检查 - 需要包含TypeManager.h才能使用TypeOf
        // 这里先不做类型检查，直接返回
        return TOptional<T>(*reinterpret_cast<const T*>(Addr));
    }

    /// 设置属性值
    template <typename T, typename ObjectType>
    bool SetValue(ObjectType* InObject, const T& InValue) const
    {
        if (InObject == nullptr)
        {
            return false;
        }
        void* Addr = GetAddress(InObject);
        if (Addr == nullptr)
        {
            return false;
        }
        // 类型检查 - 需要包含TypeManager.h才能使用TypeOf
        // 这里先不做类型检查，直接设置
        *reinterpret_cast<T*>(Addr) = InValue;
        return true;
    }

    /// 注册属性注解（链式调用）
    FProperty RegisterAttribute(FName InAttributeName, FName InAttributeValue);

    /// 检查是否有注解
    bool HasAttribute(FName InAttributeName) const
    {
        return Attributes.Contains(InAttributeName);
    }

    /// 获取注解
    FName GetAttribute(FName InAttributeName) const
    {
        const FName* Value = Attributes.Find(InAttributeName);
        return Value != nullptr ? *Value : FName();
    }
};

// 全局函数：获取属性值
template <typename T, typename ObjectType>
inline TOptional<T> GetPropertyValue(FProperty InProperty, ObjectType* InObject)
{
    if (InProperty == nullptr)
    {
        return TOptional<T>();
    }
    return InProperty->GetValue<T>(InObject);
}

template <typename T, typename ObjectType>
inline TOptional<T> GetPropertyValue(FProperty InProperty, const ObjectType* InObject)
{
    if (InProperty == nullptr)
    {
        return TOptional<T>();
    }
    return InProperty->GetValue<T>(InObject);
}

// 全局函数：设置属性值
template <typename T, typename ObjectType>
inline bool SetPropertyValue(FProperty InProperty, ObjectType* InObject, const T& InValue)
{
    if (InProperty == nullptr)
    {
        return false;
    }
    return InProperty->SetValue<T>(InObject, InValue);
}
