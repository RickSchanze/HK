#pragma once

#include "Property.h"
#include "Type.h"
#include "Core/Utility/Profiler.h"

// 前向声明，避免循环依赖
class FTypeManager;

template <typename ClassType, typename PropertyType>
inline FTypeMutable FTypeImpl::RegisterProperty(PropertyType ClassType::* InMemberPtr, const char* InName)
{
    // 计算成员指针偏移量
    Int32 Offset = static_cast<Int32>(reinterpret_cast<size_t>(&(reinterpret_cast<ClassType*>(0)->*InMemberPtr)));

    // 创建属性
    FPropertyImpl* PropertyImpl = new FPropertyImpl();
    PropertyImpl->Name = FName(InName);
    // 获取类型 - 通过外部函数调用来避免循环依赖
    // 需要在包含TypeManager.h的地方调用SetPropertyType来设置
    PropertyImpl->Type = nullptr;
    PropertyImpl->Offset = Offset;
    PropertyImpl->OwnerType = this;
    PropertyImpl->Flags = EPropertyFlags::None;

    // 检测是否是Enum字段
    if constexpr (std::is_enum_v<PropertyType>)
    {
        PropertyImpl->Flags |= EPropertyFlags::Enum;
    }

    // 检测容器类型 - 简化处理，实际需要更复杂的模板元编程
    // 这里先不实现容器检测，后续可以扩展

    // 存储属性
    Properties.Add(PropertyImpl);
    return this;
}

inline FTypeMutable FTypeImpl::RegisterParent(const FType InParentType)
{
    if (InParentType != nullptr)
    {
        Bases.Add(InParentType);
    }
    return this;
}

inline FTypeMutable FTypeImpl::RegisterAttribute(const FName InAttributeName, FStringView InAttributeValue)
{
    Attributes[InAttributeName] = InAttributeValue;
    return this;
}

template <typename EnumType>
inline FTypeMutable FTypeImpl::RegisterEnumMember(EnumType InEnumValue, const char* InName)
{
    // 创建枚举成员属性
    FPropertyImpl* PropertyImpl = new FPropertyImpl();
    PropertyImpl->Name = FName(InName);
    PropertyImpl->Type = this; // 枚举成员的类型就是枚举类型本身
    PropertyImpl->Offset =
        static_cast<Int32>(static_cast<std::underlying_type_t<EnumType>>(InEnumValue)); // EnumValue作为Offset
    PropertyImpl->OwnerType = this;
    PropertyImpl->Flags = EPropertyFlags::Enum; // 标记为枚举成员

    // 存储属性
    Properties.Add(PropertyImpl);
    return this;
}

template <typename T>
inline T* FTypeImpl::CreateInstanceT() const
{
    if (!CanCreateInstance())
    {
        return nullptr;
    }
    return New<T>();
}

template <typename T>
inline std::unique_ptr<T> FTypeImpl::CreateUnique() const
{
    if (!CanCreateInstance())
    {
        return nullptr;
    }
    // 使用 New 分配内存（会加入 Profiler 跟踪）
    T* Ptr = New<T>();
    // 创建自定义删除器，使用 Delete 释放内存
    auto Deleter = [](T* P) { Delete(P); };
    return std::unique_ptr<T>(Ptr, Deleter);
}

template <typename T>
inline std::shared_ptr<T> FTypeImpl::CreateShared() const
{
    if (!CanCreateInstance())
    {
        return nullptr;
    }
    // 使用 New 分配内存（会加入 Profiler 跟踪）
    T* Ptr = New<T>();
    // 创建自定义删除器，使用 Delete 释放内存
    auto Deleter = [](T* P) { Delete(P); };
    return std::shared_ptr<T>(Ptr, Deleter);
}
