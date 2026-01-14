//
// Created by Admin on 2025/12/27.
//

#include "HashUtility.h"
#include "Core/Container/Span.h"
#include "Core/Logging/Logger.h"
#include <fstream>
#include <xxhash.h>

HashType FHashUtility::ComputeHash(const void* Data, size_t Size)
{
    if (!Data || Size == 0)
    {
        return 0;
    }

    return XXH64(Data, Size, 0);
}

HashType FHashUtility::ComputeHash(const char* Str)
{
    if (!Str)
    {
        return 0;
    }

    return XXH64(Str, strlen(Str), 0);
}

HashType FHashUtility::ComputeFileHash(const char* FilePath)
{
    if (!FilePath)
    {
        return 0;
    }

    std::ifstream File(FilePath, std::ios::binary | std::ios::ate);
    if (!File.is_open())
    {
        HK_LOG_WARN(ELogcat::Utility, "Failed to open file for hash computation: {}", FilePath);
        return 0;
    }

    const std::streamsize FileSize = File.tellg();
    if (FileSize <= 0)
    {
        File.close();
        return 0;
    }

    File.seekg(0, std::ios::beg);

    // 使用 XXH64 的流式 API 来计算大文件的 Hash
    XXH64_state_t* State = XXH64_createState();
    if (!State)
    {
        HK_LOG_ERROR(ELogcat::Utility, "Failed to create XXH64 state");
        File.close();
        return 0;
    }

    XXH64_reset(State, 0);

    constexpr size_t BufferSize = 64 * 1024; // 64KB 缓冲区
    char             Buffer[BufferSize];

    while (File.good())
    {
        File.read(Buffer, BufferSize);
        if (const std::streamsize BytesRead = File.gcount(); BytesRead > 0)
        {
            XXH64_update(State, Buffer, static_cast<size_t>(BytesRead));
        }
    }

    const HashType Hash = XXH64_digest(State);
    XXH64_freeState(State);
    File.close();

    return Hash;
}

HashType FHashUtility::CombineHashes(const HashType* Hashes, const size_t Count)
{
    if (!Hashes || Count == 0)
    {
        return 0;
    }

    // 使用 hash 组合算法合并多个 Hash 值
    // 参考算法：将每个 value 合并到 seed 中
    constexpr HashType Add = 0x9e3779b97f4a7c15ULL;
    constexpr HashType Mul = 0x9ddfea08eb382d69ULL;

    HashType Seed = Hashes[0]; // 从第一个 hash 值开始

    // 对后续的每个 hash 值进行合并
    for (size_t i = 1; i < Count; ++i)
    {
        HashType Value = Hashes[i];
        Value *= Mul;
        Value ^= Value >> 47;
        Value *= Mul;
        Seed ^= Value + Add;
    }

    return Seed;
}

HashType FHashUtility::CombineHashes(const TSpan<const HashType> Hashes)
{
    if (Hashes.IsEmpty())
    {
        return 0;
    }

    return CombineHashes(Hashes.Data(), Hashes.Size());
}

HashType FHashUtility::CombineHashes(const std::initializer_list<HashType> Hashes)
{
    if (Hashes.size() == 0)
    {
        return 0;
    }

    return CombineHashes(Hashes.begin(), Hashes.size());
}
