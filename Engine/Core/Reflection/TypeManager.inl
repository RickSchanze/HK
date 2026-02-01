#pragma once

#include "Core/Logging/Logger.h"
#include "Core/Reflection/AnyRef.h"
#include "Core/Utility/Profiler.h"
#include "TypeManager.h"

template <typename T>
FType FTypeManager::GetType() const
{
    void* TypeId = GetTypeId<T>();
    TypeRegistererFunc RegistererFunc = nullptr;

    // 第一次查找：检查类型是否已注册，同时检查是否有注册函数
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        const FName* TypeNamePtr = TypeIdToNameMap.Find(TypeId);
        if (TypeNamePtr != nullptr)
        {
            FTypeImpl* const* Found = TypeMap.Find(*TypeNamePtr);
            if (Found != nullptr)
            {
                return *Found;
            }
        }

        // 检查是否有注册函数
        const TypeRegistererFunc* FuncPtr = TypeRegistererMap.Find(TypeId);
        if (FuncPtr != nullptr && *FuncPtr != nullptr)
        {
            RegistererFunc = *FuncPtr;
        }
    }

    // 如果找到注册函数，在锁外调用（避免死锁）
    if (RegistererFunc != nullptr)
    {
        RegistererFunc();

        // 重新查找类型
        std::lock_guard<std::mutex> Lock(Mutex);
        const FName* TypeNamePtr = TypeIdToNameMap.Find(TypeId);
        if (TypeNamePtr != nullptr)
        {
            FTypeImpl* const* Found = TypeMap.Find(*TypeNamePtr);
            if (Found != nullptr)
            {
                return *Found;
            }
        }
    }

    return nullptr;
}

template <typename T>
FTypeMutable FTypeManager::RegisterType(const char* InName)
{
    std::lock_guard<std::mutex> Lock(Mutex);

    FName TypeName(InName);

    // 检查是否已经注册（使用FName）
    FTypeImpl* const* Existing = TypeMap.Find(TypeName);
    if (Existing != nullptr)
    {
        // 返回可变指针
        return *Existing;
    }

    // 创建新的类型信息
    FTypeImpl* TypeImpl = new FTypeImpl();
    TypeImpl->Name = TypeName;
    TypeImpl->Size = static_cast<Int32>(sizeof(T));
    TypeImpl->Flags = ETypeFlags::None;

    // 检测是否是Enum
    if constexpr (IsEnumType<T>())
    {
        TypeImpl->Flags |= ETypeFlags::Enum;
        // 使用Unsafe版本，因为我们已经持有锁了
        TypeImpl->UnderlyingType = GetUnderlyingTypeForEnumUnsafe<T>();
    }

    // 如果不是Abstract或Interface，注册创建和销毁函数
    // 使用 if constexpr 检查类型是否有 IsAbstract() 方法
    // 如果类型没有IsAbstract()方法，则默认为false（可实例化）
    constexpr bool bIsAbstract = []() constexpr {
        if constexpr (requires { { T::IsAbstract() } -> std::same_as<bool>; })
        {
            return T::IsAbstract();
        }
        else
        {
            return false;
        }
    }();
    
    if constexpr (!bIsAbstract)
    {
        // 注册创建函数
        TypeCreateMap[TypeName] = []() -> void* { return New<T>(); };

        // 注册销毁函数
        TypeDestroyMap[TypeName] = [](void* InInstance)
        {
            if (InInstance != nullptr)
            {
                Delete(static_cast<T*>(InInstance));
            }
        };
    }

    // 存储类型信息
    TypeStorage.Add(TypeImpl);
    TypeMap[TypeName] = TypeImpl;
    // 建立类型唯一标识符到名称的映射
    void* TypeId = GetTypeId<T>();
    TypeIdToNameMap[TypeId] = TypeName;

    return TypeImpl;
}

template <typename T>
void FTypeManager::RegisterTypeRegistererImpl(TypeRegistererFunc InFunc)
{
    std::lock_guard<std::mutex> Lock(Mutex);
    // 使用类型唯一标识符作为key
    void* TypeId = GetTypeId<T>();
    TypeRegistererMap[TypeId] = InFunc;
}

template <typename T>
FAnyRef FTypeManager::GetValueRef(const FAnyRef& InAnyRef)
{
    return Get().GetValueRefImpl<T>(InAnyRef);
}

template <typename T>
FAnyRef FTypeManager::GetValueRefImpl(const FAnyRef& InAnyRef) const
{
    if (!InAnyRef.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Reflection, "FAnyRef is invalid in GetValueRef");
        return FAnyRef();
    }

    FType TargetType = TypeOf<T>();
    FType SourceType = InAnyRef.GetType();

    if (TargetType == nullptr || SourceType == nullptr)
    {
        HK_LOG_ERROR(ELogcat::Reflection, "Type is null in GetValueRef");
        return FAnyRef();
    }

    // 类型检查
    if (TargetType != SourceType)
    {
        HK_LOG_ERROR(ELogcat::Reflection, "Type mismatch in GetValueRef: {} != {}", TargetType->Name.GetStdString(),
                     SourceType->Name.GetStdString());
        return FAnyRef();
    }

    void* DataPtr = const_cast<void*>(InAnyRef.GetData());
    T* ValuePtr = reinterpret_cast<T*>(DataPtr);
    if (ValuePtr == nullptr)
    {
        HK_LOG_ERROR(ELogcat::Reflection, "Failed to get data pointer in GetValueRef");
        return FAnyRef();
    }

    return FAnyRef(*ValuePtr);
}

template <typename T, typename T1>
bool FTypeManager::TryGetValueImpl(const T1& InObj, T& OutValue) const
{
    FType SourceType = TypeOf<T1>();
    FType TargetType = TypeOf<T>();

    if (SourceType == nullptr || TargetType == nullptr)
    {
        HK_LOG_ERROR(ELogcat::Reflection, "Type is null in TryGetValue");
        return false;
    }

    // 尝试类型转换
    auto CastResult = Cast<T>(InObj);
    if (CastResult.IsSet())
    {
        OutValue = CastResult.GetValue();
        return true;
    }

    HK_LOG_ERROR(ELogcat::Reflection, "Type conversion failed in TryGetValue: {} -> {}",
                 SourceType->Name.GetStdString(), TargetType->Name.GetStdString());
    return false;
}

// 获取类型唯一标识符（使用static局部变量，每个类型都有唯一的地址）
template <typename T>
void* FTypeManager::GetTypeId()
{
    static char TypeId = 0;
    return &TypeId;
}

// 获取类型名 - 从TypeIdToNameMap中查找
template <typename T>
FName FTypeManager::GetTypeName() const
{
    std::lock_guard<std::mutex> Lock(Mutex);
    void* TypeId = GetTypeId<T>();
    const FName* Found = TypeIdToNameMap.Find(TypeId);
    if (Found != nullptr)
    {
        return *Found;
    }
    // 如果未找到，返回空名称
    return FName();
}
