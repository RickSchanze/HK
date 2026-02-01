#pragma once
#include "Core/String/StringView.h"
#include <cereal/cereal.hpp>
#include <concepts>
#include <type_traits> // for std::decay_t, std::declval, std::is_void_v

// 辅助函数
auto MakeNamedPair(FStringView Name, auto&& Value)
{
    return cereal::make_nvp(Name.Data(), Value);
}

// =========================================================
// 1. 极简模式 (Primitive) 适配
// =========================================================

// 定义一个辅助 Concept: 只要类型 T 不是 void
template <typename T>
concept CNotVoid = !std::is_void_v<T>;

// Concept: 检测是否存在 WritePrimitive() 且返回值不是 void
template <typename T>
concept CHasWritePrimitive = requires(T t) {
    { t.WritePrimitive() } -> CNotVoid;
};

// 辅助别名：获取 WritePrimitive 的返回类型 (去除引用和 const)
// 例如：如果 WritePrimitive 返回 const std::string&，这个就是 std::string
template <typename T>
using TPrimitiveType = std::decay_t<decltype(std::declval<T>().WritePrimitive())>;

// Concept: 检测是否存在 ReadPrimitive(Value)
// 强制要求 ReadPrimitive 接受的参数类型与 WritePrimitive 返回的类型一致
template <typename T, typename U>
concept CHasReadPrimitive = requires(T t, const U& u) {
    { t.ReadPrimitive(u) } -> std::same_as<void>;
};

namespace cereal
{
// 适配 Write_minimal
// 显式指定返回类型，帮助编译器确信它与 load_minimal 匹配
template <class Archive, class T>
    requires CHasWritePrimitive<T>
auto save_minimal(const Archive&, const T& t) -> TPrimitiveType<T>
{
    return const_cast<T&>(t).WritePrimitive();
}

// 适配 load_minimal
// 关键点：直接使用 TPrimitiveType<T> 作为参数类型，而不是泛型 U
template <class Archive, class T>
    requires CHasWritePrimitive<T> && CHasReadPrimitive<T, TPrimitiveType<T>>
void load_minimal(const Archive&, T& t, const TPrimitiveType<T>& value)
{
    t.ReadPrimitive(value);
}
} // namespace cereal

// =========================================================
// 2. 普通模式 (Object) 适配 (Write / Read)
// =========================================================

template <typename T, typename Archive>
concept CHasCustomWrite = requires(T t, Archive& ar) {
    { t.Write(ar) } -> std::same_as<void>;
};

template <typename T, typename Archive>
concept CHasCustomRead = requires(T t, Archive& ar) {
    { t.Read(ar) } -> std::same_as<void>;
};

namespace cereal
{
template <class Archive, class T>
    requires CHasCustomWrite<T, Archive>
void save(Archive& ar, const T& t)
{
    const_cast<T&>(t).Write(ar);
}

template <class Archive, class T>
    requires CHasCustomRead<T, Archive>
void load(Archive& ar, T& t)
{
    t.Read(ar);
}
} // namespace cereal

// =========================================================
// 3. Serialize 统一适配
// =========================================================

template <typename T, typename Archive>
concept CHasSerialize = requires(T t, Archive& ar) {
    { t.Serialize(ar) } -> std::same_as<void>;
};

namespace cereal
{
// 使用 save/load 函数对来适配 Serialize 方法
// 注意：不能同时提供 serialize 和 save/load，cereal 要求只能二选一
template <class Archive, class T>
    requires CHasSerialize<T, Archive>
void save(Archive& ar, const T& t)
{
    const_cast<T&>(t).Serialize(ar);
}

template <class Archive, class T>
    requires CHasSerialize<T, Archive>
void load(Archive& ar, T& t)
{
    t.Serialize(ar);
}
} // namespace cereal