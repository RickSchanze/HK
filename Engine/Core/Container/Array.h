#pragma once

#include "Core/Container/Span.h"
#include <algorithm>
#include <initializer_list>
#include <vector>

template <typename T>
class TArray
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
    using Iterator = typename std::vector<T>::iterator;
    using ConstIterator = typename std::vector<T>::const_iterator;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    TArray() = default;
    explicit TArray(SizeType InSize) : MyData(InSize) {}
    TArray(SizeType InSize, const T& InValue) : MyData(InSize, InValue) {}
    TArray(std::initializer_list<T> InitList) : MyData(InitList) {}
    template <typename InputIt>
    TArray(InputIt First, InputIt Last) : MyData(First, Last)
    {
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

    SizeType Size() const noexcept
    {
        return MyData.size();
    }
    SizeType Length() const noexcept
    {
        return MyData.size();
    }
    bool IsEmpty() const noexcept
    {
        return MyData.empty();
    }
    SizeType Capacity() const noexcept
    {
        return MyData.capacity();
    }
    SizeType MaxSize() const noexcept
    {
        return MyData.max_size();
    }

    void Reserve(SizeType NewCapacity)
    {
        MyData.reserve(NewCapacity);
    }
    void ShrinkToFit()
    {
        MyData.shrink_to_fit();
    }
    void Resize(SizeType NewSize)
    {
        MyData.resize(NewSize);
    }
    void Resize(SizeType NewSize, const T& Value)
    {
        MyData.resize(NewSize, Value);
    }
    void Clear() noexcept
    {
        MyData.clear();
    }

    void Add(const T& Value)
    {
        MyData.push_back(Value);
    }
    void Add(T&& Value)
    {
        MyData.push_back(std::move(Value));
    }
    template <typename... Args>
    void Emplace(Args&&... Args_)
    {
        MyData.emplace_back(std::forward<Args>(Args_)...);
    }
    void Append(const TArray<T>& Other)
    {
        MyData.insert(MyData.end(), Other.begin(), Other.end());
    }
    template <typename InputIt>
    void Append(InputIt First, InputIt Last)
    {
        MyData.insert(MyData.end(), First, Last);
    }

    void RemoveAt(SizeType Index)
    {
        MyData.erase(MyData.begin() + Index);
    }

    bool Remove(const T& Value)
    {
        auto It = std::find(MyData.begin(), MyData.end(), Value);
        if (It != MyData.end())
        {
            MyData.erase(It);
            return true;
        }
        return false;
    }

    template <typename Predicate>
    bool RemoveByPredicate(Predicate&& Pred)
    {
        auto It = std::find_if(MyData.begin(), MyData.end(), Pred);
        if (It != MyData.end())
        {
            MyData.erase(It);
            return true;
        }
        return false;
    }

    void RemoveAll(const T& Value)
    {
        MyData.erase(std::remove(MyData.begin(), MyData.end(), Value), MyData.end());
    }

    template <typename Predicate>
    void RemoveAllByPredicate(Predicate&& Pred)
    {
        MyData.erase(std::remove_if(MyData.begin(), MyData.end(), Pred), MyData.end());
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
        return TSpan<T>(MyData.data() + Offset, MyData.size() - Offset);
    }

    TSpan<const T> Slice(SizeType Offset) const noexcept
    {
        return TSpan<const T>(MyData.data() + Offset, MyData.size() - Offset);
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

    void Pop()
    {
        MyData.pop_back();
    }
    void PopBack()
    {
        MyData.pop_back();
    }

private:
    std::vector<T> MyData;
};
