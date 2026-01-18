#pragma once

#include "Core/Container/Span.h"
#include "Core/Utility/Macros.h"
#include "Core/Utility/Profiler.h"

#include <algorithm>
#include <cstring>

// 字节缓冲区容器类
// 提供 Read/Write 操作，支持模板和 void* 两种方式
// RAII：析构时自动销毁内存
class FBuffer
{
public:
    using SizeType         = size_t;
    using ByteType         = UInt8;
    using Pointer          = void*;
    using ConstPointer     = const void*;
    using BytePointer      = ByteType*;
    using ConstBytePointer = const ByteType*;

    // 构造函数
    FBuffer() noexcept : MyData(nullptr), MySize(0), MyCapacity(0) {}

    explicit FBuffer(SizeType InSize) : MyData(nullptr), MySize(0), MyCapacity(0)
    {
        Resize(InSize);
    }

    FBuffer(SizeType InSize, ByteType FillValue) : MyData(nullptr), MySize(0), MyCapacity(0)
    {
        Resize(InSize, FillValue);
    }

    FBuffer(const void* InData, SizeType InSize) : MyData(nullptr), MySize(0), MyCapacity(0)
    {
        if (InData != nullptr && InSize > 0)
        {
            Resize(InSize);
            std::memcpy(MyData, InData, InSize);
        }
    }

    // 拷贝构造函数
    FBuffer(const FBuffer& Other) : MyData(nullptr), MySize(0), MyCapacity(0)
    {
        if (Other.MySize > 0)
        {
            Resize(Other.MySize);
            std::memcpy(MyData, Other.MyData, Other.MySize);
        }
    }

    // 移动构造函数
    FBuffer(FBuffer&& Other) noexcept : MyData(Other.MyData), MySize(Other.MySize), MyCapacity(Other.MyCapacity)
    {
        Other.MyData     = nullptr;
        Other.MySize     = 0;
        Other.MyCapacity = 0;
    }

    // 析构函数
    ~FBuffer()
    {
        Clear();
    }

    // 拷贝赋值运算符
    FBuffer& operator=(const FBuffer& Other)
    {
        if (this != &Other)
        {
            if (Other.MySize > 0)
            {
                Resize(Other.MySize);
                std::memcpy(MyData, Other.MyData, Other.MySize);
            }
            else
            {
                Clear();
            }
        }
        return *this;
    }

    // 移动赋值运算符
    FBuffer& operator=(FBuffer&& Other) noexcept
    {
        if (this != &Other)
        {
            Clear();
            MyData           = Other.MyData;
            MySize           = Other.MySize;
            MyCapacity       = Other.MyCapacity;
            Other.MyData     = nullptr;
            Other.MySize     = 0;
            Other.MyCapacity = 0;
        }
        return *this;
    }

    // 数据访问
    BytePointer Data() noexcept
    {
        return MyData;
    }

    ConstBytePointer Data() const noexcept
    {
        return MyData;
    }

    Pointer GetData() noexcept
    {
        return static_cast<Pointer>(MyData);
    }

    ConstPointer GetData() const noexcept
    {
        return static_cast<ConstPointer>(MyData);
    }

    // 大小相关
    SizeType Size() const noexcept
    {
        return MySize;
    }

    SizeType Length() const noexcept
    {
        return MySize;
    }

    SizeType Capacity() const noexcept
    {
        return MyCapacity;
    }

    bool IsEmpty() const noexcept
    {
        return MySize == 0;
    }

    // 容量管理
    void Reserve(SizeType NewCapacity)
    {
        if (NewCapacity > MyCapacity)
        {
            Reallocate(NewCapacity);
        }
    }

    void Resize(SizeType NewSize)
    {
        if (NewSize > MyCapacity)
        {
            Reallocate(NewSize);
        }
        MySize = NewSize;
    }

    void Resize(SizeType NewSize, ByteType FillValue)
    {
        SizeType OldSize = MySize;
        Resize(NewSize);
        if (NewSize > OldSize)
        {
            std::memset(MyData + OldSize, FillValue, NewSize - OldSize);
        }
    }

    void ShrinkToFit()
    {
        if (MySize < MyCapacity)
        {
            Reallocate(MySize);
        }
    }

    void Clear() noexcept
    {
        if (MyData != nullptr)
        {
            HK_PROFILE_FREE(MyData);
            delete[] MyData;
            MyData     = nullptr;
            MySize     = 0;
            MyCapacity = 0;
        }
    }

    // Read 操作 - 模板版本
    template <typename T>
    bool Read(SizeType Offset, T& OutValue) const noexcept
    {
        if (Offset + sizeof(T) > MySize)
        {
            return false;
        }
        std::memcpy(&OutValue, MyData + Offset, sizeof(T));
        return true;
    }

    template <typename T>
    bool Read(SizeType Offset, T* OutData, SizeType Count) const noexcept
    {
        SizeType TotalSize = sizeof(T) * Count;
        if (Offset + TotalSize > MySize)
        {
            return false;
        }
        std::memcpy(OutData, MyData + Offset, TotalSize);
        return true;
    }

    // Read 操作 - void* 版本
    bool Read(SizeType Offset, void* OutData, SizeType InSize) const noexcept
    {
        if (Offset + InSize > MySize)
        {
            return false;
        }
        std::memcpy(OutData, MyData + Offset, InSize);
        return true;
    }

    // Write 操作 - 模板版本
    template <typename T>
    bool Write(SizeType Offset, const T& InValue) noexcept
    {
        if (Offset + sizeof(T) > MySize)
        {
            return false;
        }
        std::memcpy(MyData + Offset, &InValue, sizeof(T));
        return true;
    }

    template <typename T>
    bool Write(SizeType Offset, const T* InData, SizeType Count) noexcept
    {
        SizeType TotalSize = sizeof(T) * Count;
        if (Offset + TotalSize > MySize)
        {
            return false;
        }
        std::memcpy(MyData + Offset, InData, TotalSize);
        return true;
    }

    // Write 操作 - void* 版本
    bool Write(SizeType Offset, const void* InData, SizeType InSize) noexcept
    {
        if (Offset + InSize > MySize)
        {
            return false;
        }
        std::memcpy(MyData + Offset, InData, InSize);
        return true;
    }

    // Append 操作 - 模板版本
    template <typename T>
    void Append(const T& InValue)
    {
        SizeType OldSize = MySize;
        Resize(MySize + sizeof(T));
        std::memcpy(MyData + OldSize, &InValue, sizeof(T));
    }

    template <typename T>
    void Append(const T* InData, SizeType Count)
    {
        SizeType TotalSize = sizeof(T) * Count;
        SizeType OldSize   = MySize;
        Resize(MySize + TotalSize);
        std::memcpy(MyData + OldSize, InData, TotalSize);
    }

    // Append 操作 - void* 版本
    void Append(const void* InData, SizeType InSize)
    {
        if (InData != nullptr && InSize > 0)
        {
            SizeType OldSize = MySize;
            Resize(MySize + InSize);
            std::memcpy(MyData + OldSize, InData, InSize);
        }
    }

    void Append(const FBuffer& Other)
    {
        if (Other.MySize > 0)
        {
            Append(Other.MyData, Other.MySize);
        }
    }

    // 填充操作
    void Fill(ByteType Value)
    {
        if (MySize > 0)
        {
            std::memset(MyData, Value, MySize);
        }
    }

    void Fill(SizeType Offset, SizeType Count, ByteType Value)
    {
        if (Offset + Count <= MySize)
        {
            std::memset(MyData + Offset, Value, Count);
        }
    }

    // 复制操作
    void Copy(SizeType DstOffset, SizeType SrcOffset, SizeType Count) noexcept
    {
        if (DstOffset + Count <= MySize && SrcOffset + Count <= MySize)
        {
            std::memmove(MyData + DstOffset, MyData + SrcOffset, Count);
        }
    }

    void CopyFrom(const FBuffer& Other, SizeType DstOffset, SizeType SrcOffset, SizeType Count) noexcept
    {
        if (DstOffset + Count <= MySize && SrcOffset + Count <= Other.MySize)
        {
            std::memcpy(MyData + DstOffset, Other.MyData + SrcOffset, Count);
        }
    }

    // 切片操作
    TSpan<ByteType> Slice(SizeType Offset, SizeType Count) noexcept
    {
        HK_ASSERT_RAW(Offset + Count <= MySize);
        return TSpan<ByteType>(MyData + Offset, Count);
    }

    TSpan<const ByteType> Slice(SizeType Offset, SizeType Count) const noexcept
    {
        HK_ASSERT_RAW(Offset + Count <= MySize);
        return TSpan<const ByteType>(MyData + Offset, Count);
    }

    TSpan<ByteType> Slice(SizeType Offset) noexcept
    {
        HK_ASSERT_RAW(Offset <= MySize);
        return TSpan<ByteType>(MyData + Offset, MySize - Offset);
    }

    TSpan<const ByteType> Slice(SizeType Offset) const noexcept
    {
        HK_ASSERT_RAW(Offset <= MySize);
        return TSpan<const ByteType>(MyData + Offset, MySize - Offset);
    }

    // 比较操作
    bool operator==(const FBuffer& Other) const noexcept
    {
        if (MySize != Other.MySize)
        {
            return false;
        }
        if (MySize == 0)
        {
            return true;
        }
        return std::memcmp(MyData, Other.MyData, MySize) == 0;
    }

    bool operator!=(const FBuffer& Other) const noexcept
    {
        return !(*this == Other);
    }

    // 索引访问
    ByteType& operator[](SizeType Index) noexcept
    {
        HK_ASSERT_RAW(Index < MySize);
        return MyData[Index];
    }

    const ByteType& operator[](SizeType Index) const noexcept
    {
        HK_ASSERT_RAW(Index < MySize);
        return MyData[Index];
    }

    ByteType& At(SizeType Index) noexcept
    {
        HK_ASSERT_RAW(Index < MySize);
        return MyData[Index];
    }

    const ByteType& At(SizeType Index) const noexcept
    {
        HK_ASSERT_RAW(Index < MySize);
        return MyData[Index];
    }

private:
    void Reallocate(SizeType NewCapacity)
    {
        if (NewCapacity == 0)
        {
            Clear();
            return;
        }

        ByteType* NewData = new ByteType[NewCapacity];
        HK_PROFILE_ALLOC(NewData, NewCapacity);

        if (MyData != nullptr)
        {
            SizeType CopySize = std::min(MySize, NewCapacity);
            std::memcpy(NewData, MyData, CopySize);
            HK_PROFILE_FREE(MyData);
            delete[] MyData;
        }

        MyData     = NewData;
        MyCapacity = NewCapacity;
    }

    ByteType* MyData;
    SizeType  MySize;
    SizeType  MyCapacity;
};
