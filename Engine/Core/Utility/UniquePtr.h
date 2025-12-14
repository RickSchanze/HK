#pragma once

#include <memory>
#include <type_traits>

template <typename T, typename Deleter = std::default_delete<T>>
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
              typename = std::enable_if_t<std::is_convertible_v<U*, T*> && (std::is_same_v<E, std::default_delete<T>> ||
                                                                            std::is_same_v<E, std::default_delete<U>>)>>
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
              typename = std::enable_if_t<std::is_convertible_v<U*, T*> && std::is_same_v<E, std::default_delete<T>>>>
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

private:
    std::unique_ptr<T, Deleter> MyPtr;

    template <typename U, typename E>
    friend class TUniquePtr;
};

// MakeUnique 函数
template <typename T, typename... Args>
TUniquePtr<T> MakeUnique(Args&&... InArgs)
{
    return TUniquePtr<T>(std::make_unique<T>(std::forward<Args>(InArgs)...));
}

// 从原始指针创建（使用自定义删除器）
template <typename T, typename Deleter>
TUniquePtr<T, Deleter> MakeUnique(T* InPtr, Deleter InDeleter)
{
    return TUniquePtr<T, Deleter>(InPtr, std::move(InDeleter));
}
