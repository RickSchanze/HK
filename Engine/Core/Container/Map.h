#pragma once

#include "Core/Utility/Macros.h"
#include "cereal/types/unordered_map.hpp"
#include <algorithm>
#include <unordered_map>

template <typename KeyType, typename ValueType>
class TMap
{
public:
    using Key = KeyType;
    using Value = ValueType;
    using MapType = std::unordered_map<KeyType, ValueType>;
    using Iterator = typename MapType::iterator;
    using ConstIterator = typename MapType::const_iterator;
    using SizeType = size_t;

    TMap() = default;
    TMap(std::initializer_list<std::pair<const KeyType, ValueType>> InitList) : Data(InitList) {}
    template <typename InputIt>
    TMap(InputIt First, InputIt Last) : Data(First, Last)
    {
    }

    Iterator begin() noexcept
    {
        return Data.begin();
    }
    Iterator end() noexcept
    {
        return Data.end();
    }
    ConstIterator begin() const noexcept
    {
        return Data.begin();
    }
    ConstIterator end() const noexcept
    {
        return Data.end();
    }
    ConstIterator cbegin() const noexcept
    {
        return Data.cbegin();
    }
    ConstIterator cend() const noexcept
    {
        return Data.cend();
    }

    ValueType* Find(const KeyType& Key) noexcept
    {
        auto It = Data.find(Key);
        if (It != Data.end())
        {
            return &It->second;
        }
        return nullptr;
    }

    const ValueType* Find(const KeyType& Key) const noexcept
    {
        auto It = Data.find(Key);
        if (It != Data.end())
        {
            return &It->second;
        }
        return nullptr;
    }

    Iterator FindIterator(const KeyType& Key) noexcept
    {
        return Data.find(Key);
    }

    ConstIterator FindIterator(const KeyType& Key) const noexcept
    {
        return Data.find(Key);
    }

    bool Contains(const KeyType& Key) const noexcept
    {
        return Data.find(Key) != Data.end();
    }

    ValueType& operator[](const KeyType& Key)
    {
        return Data[Key];
    }

    ValueType& operator[](KeyType&& Key)
    {
        return Data[std::move(Key)];
    }

    ValueType& At(const KeyType& Key)
    {
        HK_ASSERT_MSG_RAW(Data.find(Key) != Data.end(), "Key not found in map");
        return Data.at(Key);
    }

    const ValueType& At(const KeyType& Key) const
    {
        HK_ASSERT_MSG_RAW(Data.find(Key) != Data.end(), "Key not found in map");
        return Data.at(Key);
    }

    void Add(const KeyType& Key, const ValueType& Value)
    {
        Data[Key] = Value;
    }

    void Add(const KeyType& Key, ValueType&& Value)
    {
        Data[Key] = std::move(Value);
    }

    template <typename... Args>
    void Emplace(const KeyType& Key, Args&&... Args_)
    {
        Data.emplace(Key, std::forward<Args>(Args_)...);
    }

    bool Remove(const KeyType& Key)
    {
        return Data.erase(Key) > 0;
    }

    void Clear() noexcept
    {
        Data.clear();
    }

    SizeType Size() const noexcept
    {
        return Data.size();
    }

    bool IsEmpty() const noexcept
    {
        return Data.empty();
    }

    SizeType MaxSize() const noexcept
    {
        return Data.max_size();
    }

    template <typename Archive>
        requires(CHasSerialize<KeyType, Archive> && CHasSerialize<ValueType, Archive>)
    void Serialize(Archive& Ar)
    {
        Ar(Data);
    }

private:
    MapType Data;
};

template <typename T>
concept CMapKey = requires(const T& A, const T& B) {
    { A.GetHashCode() } -> std::same_as<size_t>;
    { A == B } -> std::same_as<bool>;
};

namespace std
{
template <CMapKey T>
struct hash<T>
{
    size_t operator()(const T& Key) const noexcept
    {
        return Key.GetHashCode();
    }
};
} // namespace std
