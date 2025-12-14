#pragma once

#include <expected>
#include <utility>

template <typename T, typename E>
class TExpected
{
public:
    using ValueType = T;
    using ErrorType = E;
    using ValueReference = T&;
    using ConstValueReference = const T&;
    using ErrorReference = E&;
    using ConstErrorReference = const E&;

    // 从值构造
    TExpected(const T& InValue) : MyExpected(InValue) {}

    TExpected(T&& InValue) : MyExpected(std::move(InValue)) {}

    // 从错误构造
    template <typename U = E>
    TExpected(std::unexpected<U> InError) : MyExpected(std::unexpected<E>(InError.error()))
    {
    }

    // 从 std::expected 构造
    explicit TExpected(const std::expected<T, E>& InExpected) : MyExpected(InExpected) {}

    explicit TExpected(std::expected<T, E>&& InExpected) : MyExpected(std::move(InExpected)) {}

    // 拷贝构造
    TExpected(const TExpected& Other) = default;

    // 移动构造
    TExpected(TExpected&& Other) noexcept = default;

    // 析构
    ~TExpected() = default;

    // 拷贝赋值
    TExpected& operator=(const TExpected& Other) = default;

    // 移动赋值
    TExpected& operator=(TExpected&& Other) noexcept = default;

    // 从值赋值
    TExpected& operator=(const T& InValue)
    {
        MyExpected = InValue;
        return *this;
    }

    TExpected& operator=(T&& InValue)
    {
        MyExpected = std::move(InValue);
        return *this;
    }

    // 从错误赋值
    template <typename U = E>
    TExpected& operator=(std::unexpected<U> InError)
    {
        MyExpected = std::unexpected<E>(InError.error());
        return *this;
    }

    // 检查是否有值
    bool HasValue() const noexcept
    {
        return MyExpected.has_value();
    }

    // 检查是否有错误
    bool HasError() const noexcept
    {
        return !MyExpected.has_value();
    }

    // 检查是否成功
    bool IsOk() const noexcept
    {
        return MyExpected.has_value();
    }

    // 获取值（如果不存在则抛出异常）
    ValueReference Value()
    {
        return MyExpected.value();
    }

    ConstValueReference Value() const
    {
        return MyExpected.value();
    }

    // 获取错误（如果存在值则抛出异常）
    ErrorReference Error()
    {
        return MyExpected.error();
    }

    ConstErrorReference Error() const
    {
        return MyExpected.error();
    }

    // 获取值或默认值
    template <typename U>
    T ValueOr(U&& InDefaultValue) const&
    {
        return MyExpected.value_or(std::forward<U>(InDefaultValue));
    }

    template <typename U>
    T ValueOr(U&& InDefaultValue) &&
    {
        return std::move(MyExpected).value_or(std::forward<U>(InDefaultValue));
    }

    // 获取值或从错误转换
    template <typename U>
    T ValueOrElse(U&& InFunc) const&
    {
        if (MyExpected.has_value())
        {
            return MyExpected.value();
        }
        return InFunc(MyExpected.error());
    }

    template <typename U>
    T ValueOrElse(U&& InFunc) &&
    {
        if (MyExpected.has_value())
        {
            return std::move(MyExpected).value();
        }
        return InFunc(MyExpected.error());
    }

    // 解引用操作符（获取值）
    ValueReference operator*()
    {
        return *MyExpected;
    }

    ConstValueReference operator*() const
    {
        return *MyExpected;
    }

    // 成员访问操作符（获取值）
    T* operator->()
    {
        return MyExpected.operator->();
    }

    const T* operator->() const
    {
        return MyExpected.operator->();
    }

    // 布尔转换
    explicit operator bool() const noexcept
    {
        return MyExpected.has_value();
    }

    // 交换
    void Swap(TExpected& Other) noexcept
    {
        MyExpected.swap(Other.MyExpected);
    }

    // 获取底层 std::expected
    const std::expected<T, E>& GetExpected() const noexcept
    {
        return MyExpected;
    }

    std::expected<T, E>& GetExpected() noexcept
    {
        return MyExpected;
    }

    // 比较操作符
    bool operator==(const TExpected& Other) const
    {
        return MyExpected == Other.MyExpected;
    }

    bool operator!=(const TExpected& Other) const
    {
        return MyExpected != Other.MyExpected;
    }

private:
    std::expected<T, E> MyExpected;
};

// MakeExpected 函数（从值）
template <typename T, typename E>
TExpected<T, E> MakeExpected(const T& InValue)
{
    return TExpected<T, E>(InValue);
}

template <typename T, typename E>
TExpected<T, E> MakeExpected(T&& InValue)
{
    return TExpected<T, E>(std::move(InValue));
}

// MakeExpected 函数（从错误）
template <typename T, typename E>
TExpected<T, E> MakeExpectedError(const E& InError)
{
    return TExpected<T, E>(std::unexpected<E>(InError));
}

template <typename T, typename E>
TExpected<T, E> MakeExpectedError(E&& InError)
{
    return TExpected<T, E>(std::unexpected<E>(std::move(InError)));
}
