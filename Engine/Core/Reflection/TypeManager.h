#pragma once

#include "Core/Container/Array.h"
#include "Core/Container/Map.h"
#include "Core/Logging/Logger.h"
#include "Core/Reflection/Property.h"
#include "Core/Reflection/Type.h"
#include "Core/String/Name.h"
#include "Core/Utility/Macros.h"
#include "Core/Utility/Optional.h"
#include <functional>
#include <mutex>
#include <type_traits>

// 前向声明
class FAnyRef;

// 前向声明
class FTypeManager;

// TypeOf模板函数 - 获取类型的FType
template <typename T>
inline FType TypeOf();

// Cast模板函数 - 类型安全的转换
template <typename T, typename U>
inline TOptional<T> Cast(const U& InValue);

// CastFast模板函数 - 不进行类型检查的快速转换
template <typename T, typename U>
inline T CastFast(const U& InValue);

class FTypeManager
{
public:
    using TypeRegistererFunc = void (*)();

    // 获取单例
    static FTypeManager& Get();

    // TypeOf实现
    template <typename T>
    static FType TypeOf()
    {
        return Get().GetType<T>();
    }

    // 注册类型（返回TypeMutable*支持链式调用）
    template <typename T>
    static FTypeMutable Register(const char* InName)
    {
        return Get().RegisterType<T>(InName);
    }

    // 注册类型注册函数
    template <typename T>
    static void RegisterTypeRegisterer(TypeRegistererFunc InFunc)
    {
        Get().RegisterTypeRegistererImpl<T>(InFunc);
    }

    // 获取类型
    template <typename T>
    FType GetType() const;

    // 注册类型实现（返回TypeMutable*）
    template <typename T>
    FTypeMutable RegisterType(const char* InName);

    // 注册类型注册函数实现
    template <typename T>
    void RegisterTypeRegistererImpl(TypeRegistererFunc InFunc);

    // 根据名称查找类型
    FType FindTypeByName(FName InName) const;

    // 根据名称查找类型（字符串版本）
    FType FindTypeByName(const char* InName) const;

    // GetValueRef - 从AnyRef获取值引用（实现移到.inl文件）
    template <typename T>
    static FAnyRef GetValueRef(const FAnyRef& InAnyRef);

    // TryGetValue - 尝试从对象获取值
    template <typename T, typename T1>
    static bool TryGetValue(const T1& InObj, T& OutValue)
    {
        return Get().TryGetValueImpl<T, T1>(InObj, OutValue);
    }

    // 类型创建和销毁函数类型
    using TypeCreateFunc = void* (*)();
    using TypeDestroyFunc = void (*)(void*);

    // 获取创建函数（内部使用）
    TypeCreateFunc GetCreateFunc(FName InTypeName) const;

    // 获取销毁函数（内部使用）
    TypeDestroyFunc GetDestroyFunc(FName InTypeName) const;

    // 获取类型唯一标识符（使用static局部变量）
    template <typename T>
    static void* GetTypeId();

    // 获取类型名（内部使用）
    template <typename T>
    FName GetTypeName() const;

    FTypeManager(const FTypeManager&) = delete;
    FTypeManager& operator=(const FTypeManager&) = delete;
private:
    FTypeManager() = default;
    ~FTypeManager() = default;


    // 内部实现
    template <typename T>
    FAnyRef GetValueRefImpl(const FAnyRef& InAnyRef) const;

    template <typename T, typename T1>
    bool TryGetValueImpl(const T1& InObj, T& OutValue) const;

    // 类型检测辅助函数
    template <typename T>
    static constexpr bool IsEnumType()
    {
        return std::is_enum_v<T>;
    }

    // 获取枚举的底层类型（内部版本，假设已经持有锁）
    template <typename T>
    FType GetUnderlyingTypeForEnumUnsafe() const
    {
        if constexpr (std::is_enum_v<T>)
        {
            using UnderlyingType = std::underlying_type_t<T>;
            // 直接查找，不通过GetType（避免重复加锁）
            void* TypeId = GetTypeId<UnderlyingType>();
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
    static FType GetUnderlyingTypeForEnum()
    {
        if constexpr (std::is_enum_v<T>)
        {
            using UnderlyingType = std::underlying_type_t<T>;
            return TypeOf<UnderlyingType>();
        }
        return nullptr;
    }

    // 容器类型检测
    template <typename T>
    static bool IsArrayType()
    {
        // 检测是否是TArray
        return false; // 需要特化
    }

    template <typename T>
    static bool IsFixedArrayType()
    {
        return false; // 需要特化
    }

    template <typename T>
    static bool IsMapType()
    {
        return false; // 需要特化
    }

    // 存储类型信息（使用FName作为key）
    TMap<FName, FTypeImpl*> TypeMap;
    TArray<FTypeImpl*> TypeStorage;
    TMap<FName, TypeCreateFunc> TypeCreateMap;
    TMap<FName, TypeDestroyFunc> TypeDestroyMap;
    // 类型唯一标识符到注册函数的映射（使用void*作为唯一标识，通过static局部变量生成）
    TMap<void*, TypeRegistererFunc> TypeRegistererMap;
    // 类型唯一标识符到名称的映射（使用void*作为唯一标识，通过static局部变量生成）
    TMap<void*, FName> TypeIdToNameMap;
    mutable std::mutex Mutex;
};

// TypeOf模板函数实现
template <typename T>
inline FType TypeOf()
{
    return FTypeManager::TypeOf<T>();
}

// Cast实现 - 类型安全的转换
template <typename T, typename U>
inline TOptional<T> Cast(const U& InValue)
{
    using TDecayed = std::decay_t<T>;
    using UDecayed = std::decay_t<U>;

    // 如果类型相同，直接返回
    if constexpr (std::is_same_v<TDecayed, UDecayed>)
    {
        return TOptional<T>(static_cast<T>(InValue));
    }

    // 数字类型转换
    if constexpr (std::is_arithmetic_v<TDecayed> && std::is_arithmetic_v<UDecayed>)
    {
        // Int32 -> Int64
        if constexpr (std::is_same_v<TDecayed, Int64> && std::is_same_v<UDecayed, Int32>)
        {
            return TOptional<T>(static_cast<T>(InValue));
        }
        // UInt32 -> UInt64
        if constexpr (std::is_same_v<TDecayed, UInt64> && std::is_same_v<UDecayed, UInt32>)
        {
            return TOptional<T>(static_cast<T>(InValue));
        }
        // UInt32 -> Int64
        if constexpr (std::is_same_v<TDecayed, Int64> && std::is_same_v<UDecayed, UInt32>)
        {
            return TOptional<T>(static_cast<T>(InValue));
        }
        // Int16 -> Int32, Int32 -> Int64等向上转型
        if constexpr (std::is_integral_v<TDecayed> && std::is_integral_v<UDecayed>)
        {
            if constexpr (sizeof(TDecayed) >= sizeof(UDecayed))
            {
                if constexpr (std::is_signed_v<TDecayed> == std::is_signed_v<UDecayed>)
                {
                    return TOptional<T>(static_cast<T>(InValue));
                }
                // 无符号可以转为有符号（如果目标类型更大）
                if constexpr (!std::is_signed_v<UDecayed> && std::is_signed_v<TDecayed> &&
                              sizeof(TDecayed) > sizeof(UDecayed))
                {
                    return TOptional<T>(static_cast<T>(InValue));
                }
            }
        }
        // Bool转换
        if constexpr (std::is_same_v<TDecayed, Bool> && std::is_arithmetic_v<UDecayed>)
        {
            return TOptional<T>(static_cast<T>(InValue != 0));
        }
        if constexpr (std::is_arithmetic_v<TDecayed> && std::is_same_v<UDecayed, Bool>)
        {
            return TOptional<T>(static_cast<T>(InValue ? 1 : 0));
        }
        // Float转换
        if constexpr (std::is_floating_point_v<TDecayed> && std::is_arithmetic_v<UDecayed>)
        {
            return TOptional<T>(static_cast<T>(InValue));
        }
        if constexpr (std::is_arithmetic_v<TDecayed> && std::is_floating_point_v<UDecayed>)
        {
            return TOptional<T>(static_cast<T>(InValue));
        }
    }

    // 其他类型必须相等
    return TOptional<T>();
}

// CastFast实现 - 不进行类型检查
template <typename T, typename U>
inline T CastFast(const U& InValue)
{
    return static_cast<T>(InValue);
}

// TypeManager模板方法实现
#include "TypeManager.inl"

// 包含Type.inl，此时TypeManager已完全定义
#include "Type.inl"
