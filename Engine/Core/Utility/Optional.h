#pragma once

#include <optional>
#include <type_traits>

template <typename T>
class TOptional
{
public:
    using ValueType = T;
    using Reference = T&;
    using ConstReference = const T&;
    using Pointer = T*;
    using ConstPointer = const T*;

    // 默认构造（无值状态）
    TOptional() noexcept : MyOptional() {}

    // 从 std::nullopt 构造
    TOptional(std::nullopt_t) noexcept : MyOptional() {}

    // 从值构造
    TOptional(const T& InValue) : MyOptional(InValue) {}

    TOptional(T&& InValue) : MyOptional(std::move(InValue)) {}

    // 从 std::optional 构造
    explicit TOptional(const std::optional<T>& InOptional) : MyOptional(InOptional) {}

    explicit TOptional(std::optional<T>&& InOptional) : MyOptional(std::move(InOptional)) {}

    // 拷贝构造
    TOptional(const TOptional& Other) = default;

    // 移动构造
    TOptional(TOptional&& Other) noexcept = default;

    // 析构
    ~TOptional() = default;

    // 拷贝赋值
    TOptional& operator=(const TOptional& Other) = default;

    // 移动赋值
    TOptional& operator=(TOptional&& Other) noexcept = default;

    // 从 std::nullopt 赋值
    TOptional& operator=(std::nullopt_t) noexcept
    {
        MyOptional = std::nullopt;
        return *this;
    }

    // 从值赋值
    TOptional& operator=(const T& InValue)
    {
        MyOptional = InValue;
        return *this;
    }

    TOptional& operator=(T&& InValue)
    {
        MyOptional = std::move(InValue);
        return *this;
    }

    // 检查是否有值
    bool IsSet() const noexcept
    {
        return MyOptional.has_value();
    }

    bool HasValue() const noexcept
    {
        return MyOptional.has_value();
    }

    // 检查是否为空
    bool IsEmpty() const noexcept
    {
        return !MyOptional.has_value();
    }

    // 获取值（如果不存在则抛出异常）
    Reference GetValue()
    {
        return MyOptional.value();
    }

    ConstReference GetValue() const
    {
        return MyOptional.value();
    }

    // 获取值或默认值
    template <typename U>
    T ValueOr(U&& InDefaultValue) const&
    {
        return MyOptional.value_or(std::forward<U>(InDefaultValue));
    }

    template <typename U>
    T ValueOr(U&& InDefaultValue) &&
    {
        return std::move(MyOptional).value_or(std::forward<U>(InDefaultValue));
    }

    // 解引用操作符
    Reference operator*()
    {
        return *MyOptional;
    }

    ConstReference operator*() const
    {
        return *MyOptional;
    }

    // 成员访问操作符
    Pointer operator->()
    {
        return MyOptional.operator->();
    }

    ConstPointer operator->() const
    {
        return MyOptional.operator->();
    }

    // 布尔转换
    explicit operator bool() const noexcept
    {
        return MyOptional.has_value();
    }

    // 重置
    void Reset() noexcept
    {
        MyOptional.reset();
    }

    // 构造值（原地构造）
    template <typename... Args>
    T& Emplace(Args&&... InArgs)
    {
        return MyOptional.emplace(std::forward<Args>(InArgs)...);
    }

    // 交换
    void Swap(TOptional& Other) noexcept
    {
        MyOptional.swap(Other.MyOptional);
    }

    // 获取底层 std::optional
    const std::optional<T>& GetOptional() const noexcept
    {
        return MyOptional;
    }

    std::optional<T>& GetOptional() noexcept
    {
        return MyOptional;
    }

    // 比较操作符
    bool operator==(const TOptional& Other) const
    {
        return MyOptional == Other.MyOptional;
    }

    bool operator!=(const TOptional& Other) const
    {
        return MyOptional != Other.MyOptional;
    }

    bool operator==(std::nullopt_t) const noexcept
    {
        return !MyOptional.has_value();
    }

    bool operator!=(std::nullopt_t) const noexcept
    {
        return MyOptional.has_value();
    }

    bool operator==(const T& InValue) const
    {
        return MyOptional == InValue;
    }

    bool operator!=(const T& InValue) const
    {
        return MyOptional != InValue;
    }

private:
    std::optional<T> MyOptional;
};

// MakeOptional 函数 - 从值构造
template <typename T>
TOptional<std::decay_t<T>> MakeOptional(T&& InValue)
{
    return TOptional<std::decay_t<T>>(std::forward<T>(InValue));
}

// MakeOptional 函数 - 从参数构造（使用 std::in_place）
template <typename T, typename... Args>
TOptional<T> MakeOptional(std::in_place_t, Args&&... InArgs)
{
    return TOptional<T>(std::in_place, std::forward<Args>(InArgs)...);
}
