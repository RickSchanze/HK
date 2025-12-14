#pragma once

#include <type_traits>

// 辅助类型特征：检查是否为非 const 引用
template <typename T>
struct IsNonConstReference : std::false_type
{
};

// 只有非 const 的引用才是非 const 引用
template <typename T>
struct IsNonConstReference<T&> : std::true_type
{
};

// const 引用不是非 const 引用
template <typename T>
struct IsNonConstReference<const T&> : std::false_type
{
};

template <typename T>
constexpr bool IsNonConstReferenceV = IsNonConstReference<T>::value;

// 辅助类型特征：检查参数包中是否包含非 const 引用
template <typename... Args>
struct HasNonConstReference;

template <>
struct HasNonConstReference<> : std::false_type
{
};

template <typename First, typename... Rest>
struct HasNonConstReference<First, Rest...>
    : std::conditional_t<IsNonConstReferenceV<First>, std::true_type, HasNonConstReference<Rest...>>
{
};

