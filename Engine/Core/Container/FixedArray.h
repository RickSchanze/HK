#pragma once

#include "Core/Container/Span.h"
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
        return MyData[Index];
    }
    ConstReference operator[](SizeType Index) const
    {
        return MyData[Index];
    }
    Reference At(SizeType Index)
    {
        return MyData.at(Index);
    }
    ConstReference At(SizeType Index) const
    {
        return MyData.at(Index);
    }
    Reference Front()
    {
        return MyData.front();
    }
    ConstReference Front() const
    {
        return MyData.front();
    }
    Reference Back()
    {
        return MyData.back();
    }
    ConstReference Back() const
    {
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
        return TSpan<T>(MyData.data() + Offset, Count);
    }

    TSpan<const T> Slice(SizeType Offset, SizeType Count) const noexcept
    {
        return TSpan<const T>(MyData.data() + Offset, Count);
    }

    TSpan<T> Slice(SizeType Offset) noexcept
    {
        return TSpan<T>(MyData.data() + Offset, N - Offset);
    }

    TSpan<const T> Slice(SizeType Offset) const noexcept
    {
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

private:
    std::array<T, N> MyData;
};
