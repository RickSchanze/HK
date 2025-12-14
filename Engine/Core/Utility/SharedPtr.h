#pragma once

#include <memory>
#include <type_traits>

template <typename T>
class TSharedPtr
{
public:
    using ElementType = T;
    using Pointer = T*;
    using ConstPointer = const T*;
    using Reference = T&;
    using ConstReference = const T&;

    // 默认构造
    TSharedPtr() noexcept : MyPtr() {}

    // 从 nullptr 构造
    TSharedPtr(std::nullptr_t) noexcept : MyPtr() {}

    // 从 std::shared_ptr 构造
    explicit TSharedPtr(std::shared_ptr<T> InPtr) : MyPtr(InPtr) {}

    // 拷贝构造
    TSharedPtr(const TSharedPtr& Other) = default;

    // 移动构造
    TSharedPtr(TSharedPtr&& Other) noexcept = default;

    // 从派生类构造
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedPtr(const TSharedPtr<U>& Other) : MyPtr(Other.MyPtr)
    {
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedPtr(TSharedPtr<U>&& Other) noexcept : MyPtr(std::move(Other.MyPtr))
    {
    }

    // 析构
    ~TSharedPtr() = default;

    // 拷贝赋值
    TSharedPtr& operator=(const TSharedPtr& Other) = default;

    // 移动赋值
    TSharedPtr& operator=(TSharedPtr&& Other) noexcept = default;

    // 从 nullptr 赋值
    TSharedPtr& operator=(std::nullptr_t) noexcept
    {
        MyPtr.reset();
        return *this;
    }

    // 从派生类赋值
    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedPtr& operator=(const TSharedPtr<U>& Other)
    {
        MyPtr = Other.MyPtr;
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedPtr& operator=(TSharedPtr<U>&& Other) noexcept
    {
        MyPtr = std::move(Other.MyPtr);
        return *this;
    }

    // 重置
    void Reset() noexcept
    {
        MyPtr.reset();
    }

    template <typename U>
    void Reset(U* InPtr)
    {
        MyPtr.reset(InPtr);
    }

    // 交换
    void Swap(TSharedPtr& Other) noexcept
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

    // 获取引用计数
    long UseCount() const noexcept
    {
        return MyPtr.use_count();
    }

    // 检查是否唯一
    bool IsUnique() const noexcept
    {
        return MyPtr.unique();
    }

    // 获取底层 std::shared_ptr
    std::shared_ptr<T> GetSharedPtr() const noexcept
    {
        return MyPtr;
    }

    // 比较操作符
    bool operator==(const TSharedPtr& Other) const noexcept
    {
        return MyPtr == Other.MyPtr;
    }

    bool operator!=(const TSharedPtr& Other) const noexcept
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

    bool operator<(const TSharedPtr& Other) const noexcept
    {
        return MyPtr < Other.MyPtr;
    }

private:
    std::shared_ptr<T> MyPtr;

    template <typename U>
    friend class TSharedPtr;
};

// MakeShared 函数
template <typename T, typename... Args>
TSharedPtr<T> MakeShared(Args&&... InArgs)
{
    return TSharedPtr<T>(std::make_shared<T>(std::forward<Args>(InArgs)...));
}

// 从原始指针创建（使用自定义删除器）
template <typename T, typename Deleter>
TSharedPtr<T> MakeShared(T* InPtr, Deleter InDeleter)
{
    return TSharedPtr<T>(std::shared_ptr<T>(InPtr, InDeleter));
}
