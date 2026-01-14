#pragma once

#include "Core/Container/Span.h"
#include "Core/Utility/Macros.h"
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <type_traits>

using HashType = UInt64;

template <typename T>
concept CGetHashCode = requires(const T& A) {
    { A.GetHashCode() } -> std::same_as<HashType>;
};

template <typename T>
concept CStdHash = requires(const T& A) {
    { std::hash<T>{}(A) } -> std::same_as<HashType>;
};

/**
 * Hash 工具类，封装 xxhash 的 Hash 计算操作
 */
class HK_API FHashUtility
{
public:
    /**
     * 计算数据的 Hash 值
     * @param Data 数据指针
     * @param Size 数据大小（字节）
     * @return Hash 值
     */
    static HashType ComputeHash(const void* Data, size_t Size);

    /**
     * 计算字符串的 Hash 值
     * @param Str 字符串
     * @return Hash 值
     */
    static HashType ComputeHash(const char* Str);

    /**
     * 计算文件的 Hash 值
     * @param FilePath 文件路径
     * @return Hash 值，如果文件不存在或读取失败则返回 0
     */
    static HashType ComputeFileHash(const char* FilePath);

    /**
     * 合并多个 Hash 值为一个 Hash 值
     * @param Hashes Hash 值数组指针
     * @param Count Hash 值数量
     * @return 合并后的 Hash 值，如果 Hashes 为空或 Count 为 0 则返回 0
     */
    static HashType CombineHashes(const HashType* Hashes, size_t Count);

    /**
     * 合并多个 Hash 值为一个 Hash 值（使用 TSpan）
     * @param Hashes Hash 值数组的 Span
     * @return 合并后的 Hash 值，如果 Hashes 为空则返回 0
     */
    static HashType CombineHashes(TSpan<const HashType> Hashes);

    /**
     * 合并多个 Hash 值为一个 Hash 值（使用初始化列表）
     * @param Hashes Hash 值初始化列表
     * @return 合并后的 Hash 值，如果 Hashes 为空则返回 0
     */
    static HashType CombineHashes(std::initializer_list<HashType> Hashes);

    /**
     * 合并多个 Hash 值为一个 Hash 值（模板可变参数版本）
     * 可以直接传入多个 Hash 值，无需使用容器
     * @param First 第一个 Hash 值
     * @param Rest 剩余的 Hash 值（可变参数）
     * @return 合并后的 Hash 值
     *
     * @example
     * UInt64 hash1 = 123;
     * UInt64 hash2 = 456;
     * UInt64 hash3 = 789;
     * UInt64 combined = FHashUtility::CombineHashes(hash1, hash2, hash3);
     */
    template <typename... Args>
    static HashType CombineHashes(HashType First, Args... Rest)
    {
        if constexpr (sizeof...(Rest) == 0)
        {
            // 只有一个参数，直接返回
            return First;
        }
        else
        {
            // 多个参数，在栈上创建数组并调用基础版本
            const HashType Hashes[] = {First, Rest...};
            return CombineHashes(Hashes, sizeof...(Rest) + 1);
        }
    }

    template <typename T>
    static HashType GetHashCode(T Value)
    {
        if constexpr (std::is_pointer_v<T>)
        {
            return std::hash<T>{}(Value);
        }
        else if constexpr (CGetHashCode<T>)
        {
            return Value.GetHashCode();
        }
        else if constexpr (CStdHash<T>)
        {
            return std::hash<T>{}(Value);
        }
        else
        {
            static_assert(false, "Unsupported type");
            return 0;
        }
    }
};
