#pragma once

#include "Core/Utility/Macros.h"
#include <cstddef>
#include <list>
#include <unordered_map>
#include <utility>

// 默认容量
constexpr size_t DEFAULT_LRU_CACHE_CAPACITY = 16;

template <typename KeyType, typename ValueType, typename KeyComp = std::equal_to<KeyType>>
class TLruCache
{
public:
    using Key = KeyType;
    using Value = ValueType;
    using KeyCompare = KeyComp;
    using SizeType = size_t;
    
    // 存储键值对的类型
    using KeyValuePair = std::pair<const KeyType, ValueType>;
    
    // 链表节点类型（需要存储键值对）
    using NodeType = std::pair<KeyType, ValueType>;
    using ListType = std::list<NodeType>;
    using ListIterator = typename ListType::iterator;
    using ConstListIterator = typename ListType::const_iterator;
    
    // 哈希表类型：键 -> 链表迭代器
    using MapType = std::unordered_map<KeyType, ListIterator>;
    
    // 键值对引用包装类（用于迭代器和结构化绑定）
    class KeyValueRef
    {
    public:
        KeyValueRef(ListIterator It) : MyIt(It) {}
        
        const KeyType& first() const { return MyIt->first; }
        ValueType& second() { return MyIt->second; }
        const ValueType& second() const { return MyIt->second; }
        
        // 支持结构化绑定的 get 函数
        template <std::size_t I>
        auto& get() const
        {
            if constexpr (I == 0)
                return MyIt->first;
            else if constexpr (I == 1)
                return MyIt->second;
        }
        
    private:
        ListIterator MyIt;
        friend class TLruCache;
    };
    
    class ConstKeyValueRef
    {
    public:
        ConstKeyValueRef(ConstListIterator It) : MyIt(It) {}
        
        const KeyType& first() const { return MyIt->first; }
        const ValueType& second() const { return MyIt->second; }
        
        template <std::size_t I>
        const auto& get() const
        {
            if constexpr (I == 0)
                return MyIt->first;
            else if constexpr (I == 1)
                return MyIt->second;
        }
        
    private:
        ConstListIterator MyIt;
        friend class TLruCache;
    };
    
    // 迭代器：遍历链表，返回键值对的引用
    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = KeyValuePair;
        using difference_type = ptrdiff_t;
        using pointer = KeyValuePair*;
        using reference = KeyValueRef;
        
        explicit Iterator(ListIterator It) : MyIt(It) {}
        
        Iterator& operator++()
        {
            ++MyIt;
            return *this;
        }
        
        Iterator operator++(int)
        {
            Iterator Temp = *this;
            ++MyIt;
            return Temp;
        }
        
        // 返回键值对的引用包装
        KeyValueRef operator*() const
        {
            return KeyValueRef(MyIt);
        }
        
        bool operator==(const Iterator& Other) const
        {
            return MyIt == Other.MyIt;
        }
        
        bool operator!=(const Iterator& Other) const
        {
            return MyIt != Other.MyIt;
        }
        
    private:
        ListIterator MyIt;
        friend class TLruCache;
    };
    
    class ConstIterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = KeyValuePair;
        using difference_type = ptrdiff_t;
        using pointer = const KeyValuePair*;
        using reference = ConstKeyValueRef;
        
        explicit ConstIterator(ConstListIterator It) : MyIt(It) {}
        
        ConstIterator& operator++()
        {
            ++MyIt;
            return *this;
        }
        
        ConstIterator operator++(int)
        {
            ConstIterator Temp = *this;
            ++MyIt;
            return Temp;
        }
        
        ConstKeyValueRef operator*() const
        {
            return ConstKeyValueRef(MyIt);
        }
        
        bool operator==(const ConstIterator& Other) const
        {
            return MyIt == Other.MyIt;
        }
        
        bool operator!=(const ConstIterator& Other) const
        {
            return MyIt != Other.MyIt;
        }
        
    private:
        ConstListIterator MyIt;
        friend class TLruCache;
    };

    // 默认构造函数，使用默认容量
    TLruCache() : MyCapacity(DEFAULT_LRU_CACHE_CAPACITY) {}
    
    // 指定容量的构造函数
    explicit TLruCache(SizeType InCapacity) : MyCapacity(InCapacity) {}
    
    // 添加键值对
    void Add(const KeyType& Key, const ValueType& Value)
    {
        // 查找是否已存在
        auto MapIt = MyMap.find(Key);
        if (MapIt != MyMap.end())
        {
            // 已存在，更新值并移到链表头部
            MapIt->second->second = Value;
            MyList.splice(MyList.begin(), MyList, MapIt->second);
            return;
        }
        
        // 不存在，需要添加
        // 如果容量已满，删除最久未使用的（链表末尾）
        if (MyList.size() >= MyCapacity)
        {
            // 删除链表末尾的元素
            auto LastIt = --MyList.end();
            MyMap.erase(LastIt->first);
            MyList.pop_back();
        }
        
        // 在链表头部添加新元素
        MyList.emplace_front(Key, Value);
        MyMap[Key] = MyList.begin();
    }
    
    void Add(const KeyType& Key, ValueType&& Value)
    {
        auto MapIt = MyMap.find(Key);
        if (MapIt != MyMap.end())
        {
            MapIt->second->second = std::move(Value);
            MyList.splice(MyList.begin(), MyList, MapIt->second);
            return;
        }
        
        if (MyList.size() >= MyCapacity)
        {
            auto LastIt = --MyList.end();
            MyMap.erase(LastIt->first);
            MyList.pop_back();
        }
        
        MyList.emplace_front(Key, std::move(Value));
        MyMap[Key] = MyList.begin();
    }
    
    // 添加未初始化的值，返回引用
    ValueType& AddUninitialized(const KeyType& Key)
    {
        auto MapIt = MyMap.find(Key);
        if (MapIt != MyMap.end())
        {
            // 已存在，移到链表头部并返回引用
            MyList.splice(MyList.begin(), MyList, MapIt->second);
            return MapIt->second->second;
        }
        
        // 不存在，需要添加
        if (MyList.size() >= MyCapacity)
        {
            auto LastIt = --MyList.end();
            MyMap.erase(LastIt->first);
            MyList.pop_back();
        }
        
        // 在链表头部添加新元素（值未初始化）
        MyList.emplace_front(Key, ValueType{});
        MyMap[Key] = MyList.begin();
        return MyList.begin()->second;
    }
    
    // 查找，返回指针（如果不存在返回 nullptr）
    ValueType* Find(const KeyType& Key) noexcept
    {
        auto MapIt = MyMap.find(Key);
        if (MapIt != MyMap.end())
        {
            // 找到，移到链表头部（更新访问顺序）
            MyList.splice(MyList.begin(), MyList, MapIt->second);
            return &MapIt->second->second;
        }
        return nullptr;
    }
    
    const ValueType* Find(const KeyType& Key) const noexcept
    {
        auto MapIt = MyMap.find(Key);
        if (MapIt != MyMap.end())
        {
            // const 版本不能修改访问顺序，但可以返回指针
            return &MapIt->second->second;
        }
        return nullptr;
    }
    
    // 迭代器支持
    Iterator begin() noexcept
    {
        return Iterator(MyList.begin());
    }
    
    Iterator end() noexcept
    {
        return Iterator(MyList.end());
    }
    
    ConstIterator begin() const noexcept
    {
        return ConstIterator(MyList.begin());
    }
    
    ConstIterator end() const noexcept
    {
        return ConstIterator(MyList.end());
    }
    
    ConstIterator cbegin() const noexcept
    {
        return ConstIterator(MyList.cbegin());
    }
    
    ConstIterator cend() const noexcept
    {
        return ConstIterator(MyList.cend());
    }
    
    // 容量和大小
    SizeType Size() const noexcept
    {
        return MyList.size();
    }
    
    SizeType Capacity() const noexcept
    {
        return MyCapacity;
    }
    
    bool IsEmpty() const noexcept
    {
        return MyList.empty();
    }
    
    // 清空
    void Clear() noexcept
    {
        MyList.clear();
        MyMap.clear();
    }
    
    // 检查是否包含键
    bool Contains(const KeyType& Key) const noexcept
    {
        return MyMap.find(Key) != MyMap.end();
    }

private:
    SizeType MyCapacity;           // 容量
    ListType MyList;                // 双向链表，存储键值对（最近访问的在前面）
    MapType MyMap;                  // 哈希表，键 -> 链表迭代器
};

// 结构化绑定支持（C++17）
// 注意：由于 C++ 标准限制，无法为嵌套类型提供部分特化
// 结构化绑定会通过 ADL 查找 get 函数，所以只需要提供 get 函数重载即可

// get 函数重载（用于结构化绑定）
namespace std
{
template <std::size_t I, typename KeyType, typename ValueType>
auto get(const typename TLruCache<KeyType, ValueType>::KeyValueRef& Ref)
{
    return Ref.template get<I>();
}

template <std::size_t I, typename KeyType, typename ValueType>
auto get(const typename TLruCache<KeyType, ValueType>::ConstKeyValueRef& Ref)
{
    return Ref.template get<I>();
}
} // namespace std

