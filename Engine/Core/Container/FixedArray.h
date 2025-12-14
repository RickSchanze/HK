#pragma once

#include "Core/Container/Span.h"
#include "Core/Serialization/Serialization.h"
#include "Core/Utility/Macros.h"
#include "cereal/types/array.hpp"
#include <algorithm>
#include <array>
#include <initializer_list>

template <typename T, size_t N>
class TFixedArray
{
public:
    using ElementType = T;
    using ValueType = T;
    using value_type = T; // For C++20 ranges compatibility
    using SizeType = size_t;
    using DifferenceType = ptrdiff_t;
    using Pointer = T*;
    using ConstPointer = const T*;
    using Reference = T&;
    using ConstReference = const T&;
    using Iterator = typename std::array<T, N>::iterator;
    using ConstIterator = typename std::array<T, N>::const_iterator;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    TFixedArray() = default;
    TFixedArray(std::initializer_list<T> InitList)
    {
        std::copy(InitList.begin(), InitList.end(), MyData.begin());
    }

    Iterator begin() noexcept
    {
        return MyData.begin();
    }
    Iterator end() noexcept
    {
        return MyData.end();
    }
    ConstIterator begin() const noexcept
    {
        return MyData.begin();
    }
    ConstIterator end() const noexcept
    {
        return MyData.end();
    }
    ConstIterator cbegin() const noexcept
    {
        return MyData.cbegin();
    }
    ConstIterator cend() const noexcept
    {
        return MyData.cend();
    }
    ReverseIterator rbegin() noexcept
    {
        return MyData.rbegin();
    }
    ReverseIterator rend() noexcept
    {
        return MyData.rend();
    }
    ConstReverseIterator rbegin() const noexcept
    {
        return MyData.rbegin();
    }
    ConstReverseIterator rend() const noexcept
    {
        return MyData.rend();
    }
    ConstReverseIterator crbegin() const noexcept
    {
        return MyData.crbegin();
    }
    ConstReverseIterator crend() const noexcept
    {
        return MyData.crend();
    }

    Reference operator[](SizeType Index)
    {
        HK_ASSERT_RAW(Index < N);
        return MyData[Index];
    }
    ConstReference operator[](SizeType Index) const
    {
        HK_ASSERT_RAW(Index < N);
        return MyData[Index];
    }
    Reference At(SizeType Index)
    {
        HK_ASSERT_RAW(Index < N);
        return MyData.at(Index);
    }
    ConstReference At(SizeType Index) const
    {
        HK_ASSERT_RAW(Index < N);
        return MyData.at(Index);
    }
    Reference Front()
    {
        HK_ASSERT_RAW(N > 0);
        return MyData.front();
    }
    ConstReference Front() const
    {
        HK_ASSERT_RAW(N > 0);
        return MyData.front();
    }
    Reference Back()
    {
        HK_ASSERT_RAW(N > 0);
        return MyData.back();
    }
    ConstReference Back() const
    {
        HK_ASSERT_RAW(N > 0);
        return MyData.back();
    }
    Pointer Data() noexcept
    {
        return MyData.data();
    }
    ConstPointer Data() const noexcept
    {
        return MyData.data();
    }

    constexpr SizeType Size() const noexcept
    {
        return N;
    }
    constexpr SizeType Length() const noexcept
    {
        return N;
    }
    constexpr bool IsEmpty() const noexcept
    {
        return N == 0;
    }
    constexpr SizeType MaxSize() const noexcept
    {
        return N;
    }

    void Fill(const T& Value)
    {
        MyData.fill(Value);
    }

    SizeType Find(const T& Value) const noexcept
    {
        auto It = std::find(MyData.begin(), MyData.end(), Value);
        if (It != MyData.end())
        {
            return static_cast<SizeType>(It - MyData.begin());
        }
        return static_cast<SizeType>(-1);
    }

    template <typename Predicate>
    SizeType FindByPredicate(Predicate&& Pred) const noexcept
    {
        auto It = std::find_if(MyData.begin(), MyData.end(), Pred);
        if (It != MyData.end())
        {
            return static_cast<SizeType>(It - MyData.begin());
        }
        return static_cast<SizeType>(-1);
    }

    bool Contains(const T& Value) const noexcept
    {
        return Find(Value) != static_cast<SizeType>(-1);
    }

    template <typename Predicate>
    bool ContainsByPredicate(Predicate&& Pred) const noexcept
    {
        return FindByPredicate(Pred) != static_cast<SizeType>(-1);
    }

    TSpan<T> Slice(SizeType Offset, SizeType Count) noexcept
    {
        HK_ASSERT_RAW(Offset <= N);
        HK_ASSERT_RAW(Offset + Count <= N);
        return TSpan<T>(MyData.data() + Offset, Count);
    }

    TSpan<const T> Slice(SizeType Offset, SizeType Count) const noexcept
    {
        HK_ASSERT_RAW(Offset <= N);
        HK_ASSERT_RAW(Offset + Count <= N);
        return TSpan<const T>(MyData.data() + Offset, Count);
    }

    TSpan<T> Slice(SizeType Offset) noexcept
    {
        HK_ASSERT_RAW(Offset <= N);
        return TSpan<T>(MyData.data() + Offset, N - Offset);
    }

    TSpan<const T> Slice(SizeType Offset) const noexcept
    {
        HK_ASSERT_RAW(Offset <= N);
        return TSpan<const T>(MyData.data() + Offset, N - Offset);
    }

    void Sort()
    {
        std::sort(MyData.begin(), MyData.end());
    }

    template <typename Compare>
    void Sort(Compare&& Comp)
    {
        std::sort(MyData.begin(), MyData.end(), Comp);
    }

    template <typename Archive>
        requires CHasSerialize<T, Archive>
    void Serialize(Archive& Ar)
    {
        Ar(MyData);
    }

private:
    std::array<T, N> MyData;
};
