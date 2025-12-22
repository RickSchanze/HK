#pragma once

#include "Core/Serialization/Serialization.h"
#include "Core/Utility/Macros.h"
#include "cereal/types/vector.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

// 固定大小的位图
template <size_t Size>
class TBitmap
{
public:
    using SizeType = size_t;
    using DifferenceType = ptrdiff_t;

    // 位引用代理类，用于支持 operator[] 返回可修改的引用
    class BitReference
    {
    public:
        BitReference(UInt8* BytePtr, UInt8 BitMask) : MyBytePtr(BytePtr), MyBitMask(BitMask) {}

        BitReference& operator=(bool Value)
        {
            if (Value)
            {
                *MyBytePtr |= MyBitMask;
            }
            else
            {
                *MyBytePtr &= ~MyBitMask;
            }
            return *this;
        }

        BitReference& operator=(const BitReference& Other)
        {
            return *this = static_cast<bool>(Other);
        }

        operator bool() const
        {
            return (*MyBytePtr & MyBitMask) != 0;
        }

        bool operator~() const
        {
            return (*MyBytePtr & MyBitMask) == 0;
        }

        BitReference& Flip()
        {
            *MyBytePtr ^= MyBitMask;
            return *this;
        }

    private:
        UInt8* MyBytePtr;
        UInt8 MyBitMask;
    };

    // 常量位引用
    class ConstBitReference
    {
    public:
        ConstBitReference(const UInt8* BytePtr, UInt8 BitMask) : MyBytePtr(BytePtr), MyBitMask(BitMask) {}

        operator bool() const
        {
            return (*MyBytePtr & MyBitMask) != 0;
        }

        bool operator~() const
        {
            return (*MyBytePtr & MyBitMask) == 0;
        }

    private:
        const UInt8* MyBytePtr;
        UInt8 MyBitMask;
    };

    TBitmap() : MyData()
    {
        std::fill(MyData.begin(), MyData.end(), 0);
    }

    explicit TBitmap(bool InitialValue) : MyData()
    {
        std::fill(MyData.begin(), MyData.end(), InitialValue ? 0xFF : 0x00);
    }

    TBitmap(std::initializer_list<bool> InitList) : MyData()
    {
        std::fill(MyData.begin(), MyData.end(), 0);
        SizeType Index = 0;
        for (bool Value : InitList)
        {
            if (Index >= SizeValue)
                break;
            Set(Index, Value);
            ++Index;
        }
    }

    // 访问操作
    BitReference operator[](SizeType Index)
    {
        HK_ASSERT_RAW(Index < SizeValue);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        return BitReference(&MyData[ByteIndex], static_cast<UInt8>(1U << BitIndex));
    }

    ConstBitReference operator[](SizeType Index) const
    {
        HK_ASSERT_RAW(Index < SizeValue);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        return ConstBitReference(&MyData[ByteIndex], static_cast<UInt8>(1U << BitIndex));
    }

    BitReference At(SizeType Index)
    {
        HK_ASSERT_RAW(Index < SizeValue);
        return (*this)[Index];
    }

    ConstBitReference At(SizeType Index) const
    {
        HK_ASSERT_RAW(Index < SizeValue);
        return (*this)[Index];
    }

    // 位操作
    void Set(SizeType Index)
    {
        HK_ASSERT_RAW(Index < SizeValue);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        MyData[ByteIndex] |= static_cast<UInt8>(1U << BitIndex);
    }

    void Set(SizeType Index, bool Value)
    {
        HK_ASSERT_RAW(Index < SizeValue);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        if (Value)
        {
            MyData[ByteIndex] |= static_cast<UInt8>(1U << BitIndex);
        }
        else
        {
            MyData[ByteIndex] &= static_cast<UInt8>(~(1U << BitIndex));
        }
    }

    void Clear(SizeType Index)
    {
        HK_ASSERT_RAW(Index < SizeValue);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        MyData[ByteIndex] &= static_cast<UInt8>(~(1U << BitIndex));
    }

    void Flip(SizeType Index)
    {
        HK_ASSERT_RAW(Index < SizeValue);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        MyData[ByteIndex] ^= static_cast<UInt8>(1U << BitIndex);
    }

    bool Test(SizeType Index) const
    {
        HK_ASSERT_RAW(Index < SizeValue);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        return (MyData[ByteIndex] & static_cast<UInt8>(1U << BitIndex)) != 0;
    }

    bool Get(SizeType Index) const
    {
        return Test(Index);
    }

    // 批量操作
    void SetAll()
    {
        std::fill(MyData.begin(), MyData.end(), 0xFF);
        // 清除超出 SizeValue 的位
        const SizeType LastByteBits = SizeValue % 8;
        if (LastByteBits > 0)
        {
            const UInt8 Mask = static_cast<UInt8>((1U << LastByteBits) - 1);
            MyData[ByteCount() - 1] &= Mask;
        }
    }

    void ClearAll()
    {
        std::fill(MyData.begin(), MyData.end(), 0);
    }

    void FlipAll()
    {
        for (auto& Byte : MyData)
        {
            Byte = ~Byte;
        }
        // 清除超出 SizeValue 的位
        const SizeType LastByteBits = SizeValue % 8;
        if (LastByteBits > 0)
        {
            const UInt8 Mask = static_cast<UInt8>((1U << LastByteBits) - 1);
            MyData[ByteCount() - 1] &= Mask;
        }
    }

    void Fill(bool Value)
    {
        std::fill(MyData.begin(), MyData.end(), Value ? 0xFF : 0x00);
        // 清除超出 SizeValue 的位
        const SizeType LastByteBits = SizeValue % 8;
        if (LastByteBits > 0 && Value)
        {
            const UInt8 Mask = static_cast<UInt8>((1U << LastByteBits) - 1);
            MyData[ByteCount() - 1] &= Mask;
        }
    }

    // 查询操作
    constexpr SizeType Size() const noexcept
    {
        return SizeValue;
    }

    constexpr SizeType Length() const noexcept
    {
        return SizeValue;
    }

    constexpr bool IsEmpty() const noexcept
    {
        return SizeValue == 0;
    }

    constexpr SizeType MaxSize() const noexcept
    {
        return SizeValue;
    }

    SizeType Count() const noexcept
    {
        SizeType Count = 0;
        for (SizeType I = 0; I < SizeValue; ++I)
        {
            if (Test(I))
            {
                ++Count;
            }
        }
        return Count;
    }

    SizeType Count(bool Value) const noexcept
    {
        return Value ? Count() : (SizeValue - Count());
    }

    bool Any() const noexcept
    {
        for (const auto& Byte : MyData)
        {
            if (Byte != 0)
            {
                return true;
            }
        }
        return false;
    }

    bool All() const noexcept
    {
        for (SizeType I = 0; I < SizeValue; ++I)
        {
            if (!Test(I))
            {
                return false;
            }
        }
        return true;
    }

    bool None() const noexcept
    {
        return !Any();
    }

    SizeType FindFirstSet() const noexcept
    {
        for (SizeType I = 0; I < SizeValue; ++I)
        {
            if (Test(I))
            {
                return I;
            }
        }
        return static_cast<SizeType>(-1);
    }

    SizeType FindFirstClear() const noexcept
    {
        for (SizeType I = 0; I < SizeValue; ++I)
        {
            if (!Test(I))
            {
                return I;
            }
        }
        return static_cast<SizeType>(-1);
    }

    // 序列化
    template <typename Archive>
    void Serialize(Archive& Ar)
    {
        Ar(MyData);
    }

private:
    static constexpr SizeType SizeValue = Size;
    constexpr SizeType ByteCount() const noexcept
    {
        return (SizeValue + 7) / 8;
    }

    std::array<UInt8, (SizeValue + 7) / 8> MyData;
};

// 动态大小的位图
class FDynamicBitmap
{
public:
    using SizeType = size_t;
    using DifferenceType = ptrdiff_t;

    // 位引用代理类
    class BitReference
    {
    public:
        BitReference(UInt8* BytePtr, UInt8 BitMask) : MyBytePtr(BytePtr), MyBitMask(BitMask) {}

        BitReference& operator=(bool Value)
        {
            if (Value)
            {
                *MyBytePtr |= MyBitMask;
            }
            else
            {
                *MyBytePtr &= ~MyBitMask;
            }
            return *this;
        }

        BitReference& operator=(const BitReference& Other)
        {
            return *this = static_cast<bool>(Other);
        }

        operator bool() const
        {
            return (*MyBytePtr & MyBitMask) != 0;
        }

        bool operator~() const
        {
            return (*MyBytePtr & MyBitMask) == 0;
        }

        BitReference& Flip()
        {
            *MyBytePtr ^= MyBitMask;
            return *this;
        }

    private:
        UInt8* MyBytePtr;
        UInt8 MyBitMask;
    };

    // 常量位引用
    class ConstBitReference
    {
    public:
        ConstBitReference(const UInt8* BytePtr, UInt8 BitMask) : MyBytePtr(BytePtr), MyBitMask(BitMask) {}

        operator bool() const
        {
            return (*MyBytePtr & MyBitMask) != 0;
        }

        bool operator~() const
        {
            return (*MyBytePtr & MyBitMask) == 0;
        }

    private:
        const UInt8* MyBytePtr;
        UInt8 MyBitMask;
    };

    FDynamicBitmap() : MySize(0), MyData() {}

    explicit FDynamicBitmap(SizeType InSize, bool InitialValue = false) : MySize(InSize), MyData(ByteCount(InSize))
    {
        std::fill(MyData.begin(), MyData.end(), InitialValue ? 0xFF : 0x00);
        ClearExtraBits();
    }

    FDynamicBitmap(std::initializer_list<bool> InitList) : MySize(InitList.size()), MyData(ByteCount(InitList.size()))
    {
        std::fill(MyData.begin(), MyData.end(), 0);
        SizeType Index = 0;
        for (bool Value : InitList)
        {
            Set(Index, Value);
            ++Index;
        }
    }

    // 访问操作
    BitReference operator[](SizeType Index)
    {
        HK_ASSERT_RAW(Index < MySize);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        return BitReference(&MyData[ByteIndex], static_cast<UInt8>(1U << BitIndex));
    }

    ConstBitReference operator[](SizeType Index) const
    {
        HK_ASSERT_RAW(Index < MySize);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        return ConstBitReference(&MyData[ByteIndex], static_cast<UInt8>(1U << BitIndex));
    }

    BitReference At(SizeType Index)
    {
        HK_ASSERT_RAW(Index < MySize);
        return (*this)[Index];
    }

    ConstBitReference At(SizeType Index) const
    {
        HK_ASSERT_RAW(Index < MySize);
        return (*this)[Index];
    }

    // 位操作
    void Set(SizeType Index)
    {
        HK_ASSERT_RAW(Index < MySize);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        MyData[ByteIndex] |= static_cast<UInt8>(1U << BitIndex);
    }

    void Set(SizeType Index, bool Value)
    {
        HK_ASSERT_RAW(Index < MySize);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        if (Value)
        {
            MyData[ByteIndex] |= static_cast<UInt8>(1U << BitIndex);
        }
        else
        {
            MyData[ByteIndex] &= static_cast<UInt8>(~(1U << BitIndex));
        }
    }

    void Clear(SizeType Index)
    {
        HK_ASSERT_RAW(Index < MySize);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        MyData[ByteIndex] &= static_cast<UInt8>(~(1U << BitIndex));
    }

    void Flip(SizeType Index)
    {
        HK_ASSERT_RAW(Index < MySize);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        MyData[ByteIndex] ^= static_cast<UInt8>(1U << BitIndex);
    }

    bool Test(SizeType Index) const
    {
        HK_ASSERT_RAW(Index < MySize);
        const SizeType ByteIndex = Index / 8;
        const UInt8 BitIndex = static_cast<UInt8>(Index % 8);
        return (MyData[ByteIndex] & static_cast<UInt8>(1U << BitIndex)) != 0;
    }

    bool Get(SizeType Index) const
    {
        return Test(Index);
    }

    // 大小管理
    void Resize(SizeType NewSize, bool Value = false)
    {
        const SizeType OldByteCount = ByteCount(MySize);
        const SizeType NewByteCount = ByteCount(NewSize);
        MySize = NewSize;

        if (NewByteCount > OldByteCount)
        {
            MyData.resize(NewByteCount, Value ? 0xFF : 0x00);
        }
        else if (NewByteCount < OldByteCount)
        {
            MyData.resize(NewByteCount);
        }

        ClearExtraBits();
        if (Value && NewSize > 0)
        {
            // 确保新添加的位被设置为 Value
            const SizeType LastByteBits = NewSize % 8;
            if (LastByteBits > 0)
            {
                const UInt8 Mask = static_cast<UInt8>((1U << LastByteBits) - 1);
                MyData[NewByteCount - 1] |= Mask;
            }
        }
    }

    void Reserve(SizeType Capacity)
    {
        MyData.reserve(ByteCount(Capacity));
    }

    void ShrinkToFit()
    {
        MyData.shrink_to_fit();
    }

    SizeType Capacity() const noexcept
    {
        return MyData.capacity() * 8;
    }

    // 批量操作
    void SetAll()
    {
        std::fill(MyData.begin(), MyData.end(), 0xFF);
        ClearExtraBits();
    }

    void ClearAll()
    {
        std::fill(MyData.begin(), MyData.end(), 0);
    }

    void FlipAll()
    {
        for (auto& Byte : MyData)
        {
            Byte = ~Byte;
        }
        ClearExtraBits();
    }

    void Fill(bool Value)
    {
        std::fill(MyData.begin(), MyData.end(), Value ? 0xFF : 0x00);
        ClearExtraBits();
    }

    // 查询操作
    SizeType Size() const noexcept
    {
        return MySize;
    }

    SizeType Length() const noexcept
    {
        return MySize;
    }

    bool IsEmpty() const noexcept
    {
        return MySize == 0;
    }

    SizeType Count() const noexcept
    {
        SizeType Count = 0;
        for (SizeType I = 0; I < MySize; ++I)
        {
            if (Test(I))
            {
                ++Count;
            }
        }
        return Count;
    }

    SizeType Count(bool Value) const noexcept
    {
        return Value ? Count() : (MySize - Count());
    }

    bool Any() const noexcept
    {
        for (const auto& Byte : MyData)
        {
            if (Byte != 0)
            {
                return true;
            }
        }
        return false;
    }

    bool All() const noexcept
    {
        for (SizeType I = 0; I < MySize; ++I)
        {
            if (!Test(I))
            {
                return false;
            }
        }
        return true;
    }

    bool None() const noexcept
    {
        return !Any();
    }

    SizeType FindFirstSet() const noexcept
    {
        for (SizeType I = 0; I < MySize; ++I)
        {
            if (Test(I))
            {
                return I;
            }
        }
        return static_cast<SizeType>(-1);
    }

    SizeType FindFirstClear() const noexcept
    {
        for (SizeType I = 0; I < MySize; ++I)
        {
            if (!Test(I))
            {
                return I;
            }
        }
        return static_cast<SizeType>(-1);
    }

    // 序列化
    template <typename Archive>
    void Serialize(Archive& Ar)
    {
        Ar(MySize);
        Ar(MyData);
    }

private:
    static constexpr SizeType ByteCount(SizeType BitCount) noexcept
    {
        return (BitCount + 7) / 8;
    }

    void ClearExtraBits()
    {
        if (MySize > 0)
        {
            const SizeType LastByteBits = MySize % 8;
            if (LastByteBits > 0)
            {
                const UInt8 Mask = static_cast<UInt8>((1U << LastByteBits) - 1);
                MyData[ByteCount(MySize) - 1] &= Mask;
            }
        }
    }

    SizeType MySize;
    std::vector<UInt8> MyData;
};
