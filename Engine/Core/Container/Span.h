#pragma once

#include "Core/Utility/Macros.h"
#include <cstddef>
#include <iterator>
#include <type_traits>

template <typename T>
class TSpan
{
public:
    using ElementType = T;
    using ValueType = std::remove_cv_t<T>;
    using SizeType = size_t;
    using DifferenceType = ptrdiff_t;
    using Pointer = T*;
    using ConstPointer = const T*;
    using Reference = T&;
    using ConstReference = const T&;
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    constexpr TSpan() noexcept : MyData(nullptr), MySize(0) {}

    constexpr TSpan(T* InData, SizeType InSize) noexcept : MyData(InData), MySize(InSize) {}

    template <typename Container>
    constexpr TSpan(Container& InContainer) noexcept : MyData(std::data(InContainer)), MySize(std::size(InContainer))
    {
    }

    template <typename U>
    constexpr TSpan(const TSpan<U>& Other) noexcept : MyData(Other.Data()), MySize(Other.Size())
    {
    }

    constexpr Iterator begin() const noexcept
    {
        return MyData;
    }
    constexpr Iterator end() const noexcept
    {
        return MyData + MySize;
    }
    constexpr ConstIterator cbegin() const noexcept
    {
        return MyData;
    }
    constexpr ConstIterator cend() const noexcept
    {
        return MyData + MySize;
    }
    constexpr ReverseIterator rbegin() const noexcept
    {
        return ReverseIterator(end());
    }
    constexpr ReverseIterator rend() const noexcept
    {
        return ReverseIterator(begin());
    }
    constexpr ConstReverseIterator crbegin() const noexcept
    {
        return ConstReverseIterator(cend());
    }
    constexpr ConstReverseIterator crend() const noexcept
    {
        return ConstReverseIterator(cbegin());
    }

    constexpr Reference operator[](SizeType Index) const noexcept
    {
        HK_ASSERT_RAW(Index < MySize);
        return MyData[Index];
    }
    constexpr Reference Front() const noexcept
    {
        HK_ASSERT_RAW(MySize > 0);
        return *MyData;
    }
    constexpr Reference Back() const noexcept
    {
        HK_ASSERT_RAW(MySize > 0);
        return MyData[MySize - 1];
    }
    constexpr Pointer Data() const noexcept
    {
        return MyData;
    }
    constexpr SizeType Size() const noexcept
    {
        return MySize;
    }
    constexpr SizeType Length() const noexcept
    {
        return MySize;
    }
    constexpr bool IsEmpty() const noexcept
    {
        return MySize == 0;
    }

    constexpr TSpan Slice(SizeType Offset, SizeType Count) const noexcept
    {
        HK_ASSERT_RAW(Offset <= MySize);
        HK_ASSERT_RAW(Offset + Count <= MySize);
        return TSpan(MyData + Offset, Count);
    }

    constexpr TSpan Slice(SizeType Offset) const noexcept
    {
        HK_ASSERT_RAW(Offset <= MySize);
        return TSpan(MyData + Offset, MySize - Offset);
    }

    constexpr SizeType Find(const T& Value) const noexcept
    {
        for (SizeType i = 0; i < MySize; ++i)
        {
            if (MyData[i] == Value)
            {
                return i;
            }
        }
        return static_cast<SizeType>(-1);
    }

    template <typename Predicate>
    constexpr SizeType FindByPredicate(Predicate&& Pred) const noexcept
    {
        for (SizeType i = 0; i < MySize; ++i)
        {
            if (Pred(MyData[i]))
            {
                return i;
            }
        }
        return static_cast<SizeType>(-1);
    }

private:
    T* MyData;
    SizeType MySize;
};

// 兼容原生数组
template <typename T, size_t N>
TSpan(T (&)[N]) -> TSpan<T>;

template <typename T, size_t N>
TSpan(const T (&)[N]) -> TSpan<const T>;
