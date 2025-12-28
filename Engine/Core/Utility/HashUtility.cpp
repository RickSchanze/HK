//
// Created by Admin on 2025/12/27.
//

#include "HashUtility.h"
#include "Core/Logging/Logger.h"
#include <fstream>
#include <xxhash.h>

FHashUtility::HashType FHashUtility::ComputeHash(const void* Data, size_t Size)
{
    if (!Data || Size == 0)
    {
        return 0;
    }

    return XXH64(Data, Size, 0);
}

FHashUtility::HashType FHashUtility::ComputeHash(const char* Str)
{
    if (!Str)
    {
        return 0;
    }

    return XXH64(Str, strlen(Str), 0);
}

FHashUtility::HashType FHashUtility::ComputeFileHash(const char* FilePath)
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
    char            Buffer[BufferSize];

    while (File.good())
    {
        File.read(Buffer, BufferSize);
        const std::streamsize BytesRead = File.gcount();
        if (BytesRead > 0)
        {
            XXH64_update(State, Buffer, static_cast<size_t>(BytesRead));
        }
    }

    const HashType Hash = XXH64_digest(State);
    XXH64_freeState(State);
    File.close();

    return Hash;
}

