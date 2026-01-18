#pragma once

#include "Core/Utility/SharedPtr.h"

#include <memory>
#include <type_traits>

// TWeakPtr - 弱引用指针，基于 std::weak_ptr
template <typename T>
class TWeakPtr
{
public:
    using ElementType = T;
    using Pointer = T*;
    using ConstPointer = const T*;

    // 默认构造
    TWeakPtr() noexcept : MyPtr() {}

    // 从 TSharedPtr 构造
    template <typename U>
    TWeakPtr(const TSharedPtr<U>& InPtr) : MyPtr(InPtr.GetSharedPtr())
    {
        static_assert(std::is_convertible_v<U*, T*>, "U* must be convertible to T*");
    }

    // 拷贝构造
    TWeakPtr(const TWeakPtr& Other) = default;

    // 移动构造
    TWeakPtr(TWeakPtr&& Other) noexcept = default;

    // 从派生类构造
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TWeakPtr(const TWeakPtr<U>& Other) : MyPtr(Other.MyPtr)
    {
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TWeakPtr(TWeakPtr<U>&& Other) noexcept : MyPtr(std::move(Other.MyPtr))
    {
    }

    // 从 std::weak_ptr 构造
    explicit TWeakPtr(std::weak_ptr<T> InPtr) : MyPtr(InPtr) {}

    // 析构
    ~TWeakPtr() = default;

    // 拷贝赋值
    TWeakPtr& operator=(const TWeakPtr& Other) = default;

    // 移动赋值
    TWeakPtr& operator=(TWeakPtr&& Other) noexcept = default;

    // 从 TSharedPtr 赋值
    template <typename U>
    TWeakPtr& operator=(const TSharedPtr<U>& InPtr)
    {
        static_assert(std::is_convertible_v<U*, T*>, "U* must be convertible to T*");
        MyPtr = InPtr.GetSharedPtr();
        return *this;
    }

    // 从派生类赋值
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TWeakPtr& operator=(const TWeakPtr<U>& Other)
    {
        MyPtr = Other.MyPtr;
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TWeakPtr& operator=(TWeakPtr<U>&& Other) noexcept
    {
        MyPtr = std::move(Other.MyPtr);
        return *this;
    }

    // 重置
    void Reset() noexcept
    {
        MyPtr.reset();
    }

    // 交换
    void Swap(TWeakPtr& Other) noexcept
    {
        MyPtr.swap(Other.MyPtr);
    }

    // 获取引用计数
    long UseCount() const noexcept
    {
        return MyPtr.use_count();
    }

    // 检查是否过期（对象是否已被销毁）
    bool IsExpired() const noexcept
    {
        return MyPtr.expired();
    }

    // 锁定，获取 TSharedPtr（如果对象还存在）
    TSharedPtr<T> Lock() const noexcept
    {
        return TSharedPtr<T>(MyPtr.lock());
    }

    // 获取底层 std::weak_ptr
    std::weak_ptr<T> GetWeakPtr() const noexcept
    {
        return MyPtr;
    }

    // 比较操作符
    bool operator==(const TWeakPtr& Other) const noexcept
    {
        return !MyPtr.owner_before(Other.MyPtr) && !Other.MyPtr.owner_before(MyPtr);
    }

    bool operator!=(const TWeakPtr& Other) const noexcept
    {
        return !(*this == Other);
    }

    bool operator<(const TWeakPtr& Other) const noexcept
    {
        return MyPtr.owner_before(Other.MyPtr);
    }

private:
    std::weak_ptr<T> MyPtr;

    template <typename U>
    friend class TWeakPtr;
};
