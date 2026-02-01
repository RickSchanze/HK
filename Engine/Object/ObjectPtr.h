#pragma once

#include <cstddef>
#include <type_traits>

template <typename T>
class TObjectPtr
{
public:
    using ElementType    = T;
    using Pointer        = T*;
    using ConstPointer   = const T*;
    using Reference      = T&;
    using ConstReference = const T&;

    // 默认构造
    TObjectPtr() noexcept : Ptr(nullptr) {}

    // 从 nullptr 构造
    TObjectPtr(std::nullptr_t) noexcept : Ptr(nullptr) {}

    // 从原始指针构造
    explicit TObjectPtr(Pointer InPtr) noexcept : Ptr(InPtr) {}

    // 从派生类指针构造
    template <typename U>
        requires std::is_base_of_v<T, U>
    TObjectPtr(U* InPtr) noexcept : Ptr(InPtr)
    {
    }

    // 拷贝构造
    TObjectPtr(const TObjectPtr& Other) noexcept : Ptr(Other.Ptr) {}

    // 从派生类 ObjectPtr 拷贝构造
    template <typename U>
        requires std::is_base_of_v<T, U>
    TObjectPtr(const TObjectPtr<U>& Other) noexcept : Ptr(Other.Get())
    {
    }

    // 移动构造
    TObjectPtr(TObjectPtr&& Other) noexcept : Ptr(Other.Ptr)
    {
        Other.Ptr = nullptr;
    }

    // 从派生类 ObjectPtr 移动构造
    template <typename U>
        requires std::is_base_of_v<T, U>
    TObjectPtr(TObjectPtr<U>&& Other) noexcept : Ptr(Other.Get())
    {
        Other.Reset();
    }

    // 析构
    ~TObjectPtr() = default;

    // 拷贝赋值
    TObjectPtr& operator=(const TObjectPtr& Other) noexcept
    {
        if (this != &Other)
        {
            Ptr = Other.Ptr;
        }
        return *this;
    }

    // 从派生类 ObjectPtr 拷贝赋值
    template <typename U>
        requires std::is_base_of_v<T, U>
    TObjectPtr& operator=(const TObjectPtr<U>& Other) noexcept
    {
        Ptr = Other.Get();
        return *this;
    }

    // 移动赋值
    TObjectPtr& operator=(TObjectPtr&& Other) noexcept
    {
        if (this != &Other)
        {
            Ptr       = Other.Ptr;
            Other.Ptr = nullptr;
        }
        return *this;
    }

    // 从派生类 ObjectPtr 移动赋值
    template <typename U>
        requires std::is_base_of_v<T, U>
    TObjectPtr& operator=(TObjectPtr<U>&& Other) noexcept
    {
        Ptr = Other.Get();
        Other.Reset();
        return *this;
    }

    // 从原始指针赋值
    TObjectPtr& operator=(Pointer InPtr) noexcept
    {
        Ptr = InPtr;
        return *this;
    }

    // 从 nullptr 赋值
    TObjectPtr& operator=(std::nullptr_t) noexcept
    {
        Ptr = nullptr;
        return *this;
    }

    // 解引用操作符
    Reference operator*() noexcept
    {
        return *Ptr;
    }

    ConstReference operator*() const noexcept
    {
        return *Ptr;
    }

    // 成员访问操作符
    Pointer operator->() noexcept
    {
        return Ptr;
    }

    ConstPointer operator->() const noexcept
    {
        return Ptr;
    }

    // 获取原始指针
    Pointer Get() noexcept
    {
        return Ptr;
    }

    ConstPointer Get() const noexcept
    {
        return Ptr;
    }

    // 重置指针
    void Reset(Pointer InPtr = nullptr) noexcept
    {
        Ptr = InPtr;
    }

    // 释放指针（不销毁对象，只返回指针）
    Pointer Release() noexcept
    {
        Pointer Temp = Ptr;
        Ptr          = nullptr;
        return Temp;
    }

    // 交换
    void Swap(TObjectPtr& Other) noexcept
    {
        Pointer Temp = Ptr;
        Ptr          = Other.Ptr;
        Other.Ptr    = Temp;
    }

    // 布尔转换（检查是否有效）
    explicit operator bool() const noexcept
    {
        return Ptr != nullptr;
    }

    // 比较操作
    bool operator==(const TObjectPtr& Other) const noexcept
    {
        return Ptr == Other.Ptr;
    }

    bool operator!=(const TObjectPtr& Other) const noexcept
    {
        return Ptr != Other.Ptr;
    }

    bool operator==(Pointer OtherPtr) const noexcept
    {
        return Ptr == OtherPtr;
    }

    bool operator!=(Pointer OtherPtr) const noexcept
    {
        return Ptr != OtherPtr;
    }

    bool operator==(std::nullptr_t) const noexcept
    {
        return Ptr == nullptr;
    }

    bool operator!=(std::nullptr_t) const noexcept
    {
        return Ptr != nullptr;
    }

    // 比较操作（与原始指针）
    friend bool operator==(Pointer Lhs, const TObjectPtr& Rhs) noexcept
    {
        return Lhs == Rhs.Ptr;
    }

    friend bool operator!=(Pointer Lhs, const TObjectPtr& Rhs) noexcept
    {
        return Lhs != Rhs.Ptr;
    }

    friend bool operator==(std::nullptr_t, const TObjectPtr& Rhs) noexcept
    {
        return nullptr == Rhs.Ptr;
    }

    friend bool operator!=(std::nullptr_t, const TObjectPtr& Rhs) noexcept
    {
        return nullptr != Rhs.Ptr;
    }

    template <typename Archive>
    void Serialize(Archive& Ar)
    {
        // TODO: ObjectPtr 序列化
    }

private:
    Pointer Ptr;
};