#pragma once

#include "Core/Container/Array.h"
#include <cstring>
#include <streambuf>

/**
 * 内存流缓冲区，直接写入到 TArray<UInt8>
 */
class FMemoryStreamBuf : public std::streambuf
{
public:
    explicit FMemoryStreamBuf(TArray<UInt8>& Buffer) : MyBuffer(Buffer) {}

protected:
    std::streamsize xsputn(const char* S, std::streamsize N) override
    {
        const size_t OldSize = MyBuffer.Size();
        MyBuffer.Resize(OldSize + static_cast<size_t>(N));
        std::memcpy(MyBuffer.Data() + OldSize, S, static_cast<size_t>(N));
        return N;
    }

    int overflow(int C) override
    {
        if (C != EOF)
        {
            MyBuffer.Add(static_cast<UInt8>(C));
        }
        return C;
    }

private:
    TArray<UInt8>& MyBuffer;
};

/**
 * 内存输出流，直接写入到 TArray<UInt8>
 */
class FMemoryOutputStream : public std::ostream
{
public:
    explicit FMemoryOutputStream(TArray<UInt8>& Buffer) : std::ostream(&MyBuf), MyBuf(Buffer) {}

private:
    FMemoryStreamBuf MyBuf;
};

/**
 * Hash 计算流缓冲区，在写入时直接计算 Hash，不存储数据
 */
class FHashStreamBuf : public std::streambuf
{
public:
    FHashStreamBuf();
    ~FHashStreamBuf() override;

    UInt64 GetHash() const;

protected:
    std::streamsize xsputn(const char* S, std::streamsize N) override;
    int overflow(int C) override;

private:
    void* MyState; // XXH64_state_t* 的不透明指针
};

/**
 * Hash 计算输出流，在写入时直接计算 Hash，不存储数据
 */
class FHashOutputStream : public std::ostream
{
public:
    FHashOutputStream() : std::ostream(&MyBuf) {}

    UInt64 GetHash() const
    {
        return MyBuf.GetHash();
    }

private:
    FHashStreamBuf MyBuf;
};
