#pragma once

#include "Core/Utility/Macros.h"
#include <cstdint>

/**
 * Hash 工具类，封装 xxhash 的 Hash 计算操作
 */
class HK_API FHashUtility
{
public:
    using HashType = UInt64;

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
};

