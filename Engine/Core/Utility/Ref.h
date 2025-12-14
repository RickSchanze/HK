#pragma once

#include <functional>

template <typename T>
class TRef
{
public:
    using Type = T;
    using Reference = T&;
    using ConstReference = const T&;

    TRef() = delete; // 不允许默认构造

    // 从引用构造
    explicit TRef(Reference InRef) : MyRef(InRef) {}

    // 从 std::reference_wrapper 构造
    explicit TRef(std::reference_wrapper<T> InWrapper) : MyRef(InWrapper.get()) {}

    // 拷贝构造
    TRef(const TRef& Other) = default;

    // 赋值操作符
    TRef& operator=(const TRef& Other) = default;

    // 获取引用
    Reference Get() noexcept
    {
        return MyRef;
    }

    ConstReference Get() const noexcept
    {
        return MyRef;
    }

    // 转换为引用
    operator Reference() noexcept
    {
        return MyRef;
    }

    operator ConstReference() const noexcept
    {
        return MyRef;
    }

    // 解引用操作符
    Reference operator*() noexcept
    {
        return MyRef;
    }

    ConstReference operator*() const noexcept
    {
        return MyRef;
    }

    // 成员访问操作符
    T* operator->() noexcept
    {
        return &MyRef;
    }

    const T* operator->() const noexcept
    {
        return &MyRef;
    }

    // 转换为 std::reference_wrapper
    operator std::reference_wrapper<T>() noexcept
    {
        return std::ref(MyRef);
    }

    operator std::reference_wrapper<const T>() const noexcept
    {
        return std::cref(MyRef);
    }

    // 比较操作符
    bool operator==(const TRef& Other) const noexcept
    {
        return &MyRef == &Other.MyRef;
    }

    bool operator!=(const TRef& Other) const noexcept
    {
        return &MyRef != &Other.MyRef;
    }

private:
    Reference MyRef;
};

// MakeRef 函数
template <typename T>
TRef<T> MakeRef(T& InRef)
{
    return TRef<T>(InRef);
}

// 从 const 引用创建 TRef<const T>
template <typename T>
TRef<const T> MakeRef(const T& InRef)
{
    return TRef<const T>(InRef);
}
