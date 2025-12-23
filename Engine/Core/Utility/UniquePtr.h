#pragma once

#include "Core/String/Name.h"
#include "Core/Utility/Profiler.h"
#include "Core/Reflection/Reflection.h"

#include <memory>
#include <type_traits>

// 无状态的删除器，使用 Delete 进行内存跟踪
// 空类，零开销，内存最小化
template <typename T>
struct TDefaultDelete
{
    constexpr TDefaultDelete() noexcept = default;
    
    template <typename U>
    constexpr TDefaultDelete(const TDefaultDelete<U>&) noexcept
    {
        static_assert(std::is_convertible_v<U*, T*>, "U* must be convertible to T*");
    }
    
    void operator()(T* Ptr) const noexcept
    {
        Delete(Ptr);
    }
};

template <typename T, typename Deleter = TDefaultDelete<T>>
class TUniquePtr
{
public:
    using ElementType = T;
    using Pointer = T*;
    using ConstPointer = const T*;
    using Reference = T&;
    using ConstReference = const T&;
    using DeleterType = Deleter;

    // 默认构造
    TUniquePtr() noexcept : MyPtr() {}

    // 从 nullptr 构造
    TUniquePtr(std::nullptr_t) noexcept : MyPtr() {}

    // 从原始指针构造
    explicit TUniquePtr(Pointer InPtr) noexcept : MyPtr(InPtr) {}

    // 从原始指针和删除器构造
    TUniquePtr(Pointer InPtr, const Deleter& InDeleter) noexcept : MyPtr(InPtr, InDeleter) {}

    TUniquePtr(Pointer InPtr, Deleter&& InDeleter) noexcept : MyPtr(InPtr, std::move(InDeleter)) {}

    // 从 std::unique_ptr 构造
    explicit TUniquePtr(std::unique_ptr<T, Deleter> InPtr) : MyPtr(std::move(InPtr)) {}

    // 移动构造
    TUniquePtr(TUniquePtr&& Other) noexcept = default;

    // 从派生类移动构造
    template <typename U, typename E = Deleter,
              typename = std::enable_if_t<std::is_convertible_v<U*, T*> && (std::is_same_v<E, TDefaultDelete<T>> ||
                                                                            std::is_same_v<E, TDefaultDelete<U>>)>>
    TUniquePtr(TUniquePtr<U, E>&& Other) noexcept : MyPtr(std::move(Other.MyPtr))
    {
    }

    // 析构
    ~TUniquePtr() = default;

    // 移动赋值
    TUniquePtr& operator=(TUniquePtr&& Other) noexcept = default;

    // 从 nullptr 赋值
    TUniquePtr& operator=(std::nullptr_t) noexcept
    {
        MyPtr.reset();
        return *this;
    }

    // 从派生类移动赋值
    template <typename U, typename E = Deleter,
              typename = std::enable_if_t<std::is_convertible_v<U*, T*> && std::is_same_v<E, TDefaultDelete<T>>>>
    TUniquePtr& operator=(TUniquePtr<U, E>&& Other) noexcept
    {
        MyPtr = std::move(Other.MyPtr);
        return *this;
    }

    // 禁止拷贝
    TUniquePtr(const TUniquePtr&) = delete;
    TUniquePtr& operator=(const TUniquePtr&) = delete;

    // 释放所有权
    Pointer Release() noexcept
    {
        return MyPtr.release();
    }

    // 重置
    void Reset() noexcept
    {
        MyPtr.reset();
    }

    void Reset(Pointer InPtr) noexcept
    {
        MyPtr.reset(InPtr);
    }

    // 交换
    void Swap(TUniquePtr& Other) noexcept
    {
        MyPtr.swap(Other.MyPtr);
    }

    // 获取原始指针
    Pointer Get() noexcept
    {
        return MyPtr.get();
    }

    ConstPointer Get() const noexcept
    {
        return MyPtr.get();
    }

    // 获取删除器
    Deleter& GetDeleter() noexcept
    {
        return MyPtr.get_deleter();
    }

    const Deleter& GetDeleter() const noexcept
    {
        return MyPtr.get_deleter();
    }

    // 解引用
    Reference operator*() noexcept
    {
        return *MyPtr;
    }

    ConstReference operator*() const noexcept
    {
        return *MyPtr;
    }

    // 成员访问
    Pointer operator->() noexcept
    {
        return MyPtr.get();
    }

    ConstPointer operator->() const noexcept
    {
        return MyPtr.get();
    }

    // 布尔转换
    explicit operator bool() const noexcept
    {
        return MyPtr != nullptr;
    }

    // 获取底层 std::unique_ptr
    std::unique_ptr<T, Deleter> GetUniquePtr() && noexcept
    {
        return std::move(MyPtr);
    }

    // 比较操作符
    bool operator==(const TUniquePtr& Other) const noexcept
    {
        return MyPtr == Other.MyPtr;
    }

    bool operator!=(const TUniquePtr& Other) const noexcept
    {
        return MyPtr != Other.MyPtr;
    }

    bool operator==(std::nullptr_t) const noexcept
    {
        return MyPtr == nullptr;
    }

    bool operator!=(std::nullptr_t) const noexcept
    {
        return MyPtr != nullptr;
    }

    bool operator<(const TUniquePtr& Other) const noexcept
    {
        return MyPtr < Other.MyPtr;
    }

    template <typename Archive>
    void Read(Archive& Ar)
    {
        if (MyPtr)
        {
            Ar(MakeNamedPair("TypeName", MyPtr->GetType()->Name));
            Ar(MakeNamedPair("Data", *MyPtr));
        }
        else
        {
            Ar(MakeNamedPair("TypeName", Names::None));
        }
    }

    template <typename Archive>
    void Write(Archive& Ar)
    {
        FName TypeName;
        Ar(MakeNamedPair("TypeName", TypeName));
        if (TypeName != Names::None)
        {
            if (const FType Type = FTypeManager::FindTypeByName(TypeName); !Type)
            {
                HK_LOG_ERROR(ELogcat::Serialize, "Can't find type {}", TypeName);
            }
            else
            {
                T* Ptr = static_cast<T*>(Type->CreateInstance());
                MyPtr.reset(Ptr);
            }
        }
    }

private:
    std::unique_ptr<T, Deleter> MyPtr;

    template <typename U, typename E>
    friend class TUniquePtr;
};

// MakeUnique 函数 - 使用 New/Delete 进行内存跟踪
// 使用默认删除器 TDefaultDelete，无状态，零开销
template <typename T, typename... Args>
TUniquePtr<T> MakeUnique(Args&&... InArgs)
{
    // 使用 New 分配内存（会加入 Profiler 跟踪）
    T* Ptr = New<T>(std::forward<Args>(InArgs)...);
    // 使用默认删除器（无状态，零开销）
    return TUniquePtr<T>(Ptr);
}

// 从原始指针创建（使用自定义删除器）
template <typename T, typename Deleter>
TUniquePtr<T, Deleter> MakeUnique(T* InPtr, Deleter InDeleter)
{
    return TUniquePtr<T, Deleter>(InPtr, std::move(InDeleter));
}
