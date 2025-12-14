#pragma once
#include "Core/String/StringView.h"

#include <cereal/cereal.hpp>

auto MakeNamedPair(FStringView Name, auto&& Value)
{
    return cereal::make_nvp(Name.Data(), Value);
}

// 1. 定义 Concepts：检测是否存在 Write(Archive) 和 Read(Archive)
template <typename T, typename Archive>
concept CHasCustomWrite = requires(T t, Archive& ar) {
    // 允许 Write 不是 const (虽然 save 传入的是 const T&，下面会处理)
    { t.Write(ar) } -> std::same_as<void>;
};

// 2. 全局 save 适配器
// 当类型 T 满足 HasCustomWrite 时，cereal 会调用这个 save
template <class Archive, class T>
    requires CHasCustomWrite<T, Archive>
void save(Archive& ar, const T& t)
{
    // cereal 的 save 签名强制要求 const T&
    // 但很多遗留代码的 Write 函数可能没有加 const 修饰
    // 这里使用 const_cast 确保最大兼容性，调用 t.Write(ar)
    const_cast<T&>(t).Write(ar);
}

template <typename T, typename Archive>
concept CHasCustomRead = requires(T t, Archive& ar) {
    { t.Read(ar) } -> std::same_as<void>;
};

// 3. 全局 load 适配器
// 当类型 T 满足 HasCustomRead 时，cereal 会调用这个 load
template <class Archive, class T>
    requires CHasCustomRead<T, Archive>
void load(Archive& ar, T& t)
{
    t.Read(ar);
}

// 1. 定义 Concept: 检测是否存在 Serialize(Archive&) 成员函数
template <typename T, typename Archive>
concept CHasSerialize = requires(T t, Archive& ar) {
    // 检测 t.Serialize(ar) 是否合法
    { t.Serialize(ar) } -> std::same_as<void>;
};

// 2. 定义全局 serialize 适配器（放在cereal命名空间中以便ADL查找）
// 只有当 T 满足 CHasSerialize 时，这个函数才会被编译和调用
namespace cereal
{
    // 非const版本（用于序列化和反序列化）
    template <class Archive, class T>
        requires CHasSerialize<T, Archive>
    void serialize(Archive& ar, T& t)
    {
        // 转发调用你的自定义函数
        t.Serialize(ar);
    }
    
    // const版本（仅用于序列化，cereal的save函数会调用这个）
    template <class Archive, class T>
        requires CHasSerialize<T, Archive>
    void serialize(Archive& ar, const T& t)
    {
        // 对于const对象，使用const_cast调用Serialize（因为Serialize可能不是const的）
        const_cast<T&>(t).Serialize(ar);
    }
}
