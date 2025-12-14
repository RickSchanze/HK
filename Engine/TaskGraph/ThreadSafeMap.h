#pragma once

#include "Core/Utility/SharedPtr.h"

#include <atomic>
#include <functional>
#include <mutex>
#include <unordered_map>

template <typename Key, typename Value>
class TThreadSafeMap
{
public:
    TThreadSafeMap() = default;
    ~TThreadSafeMap() = default;

    // 禁止拷贝
    TThreadSafeMap(const TThreadSafeMap&) = delete;
    TThreadSafeMap& operator=(const TThreadSafeMap&) = delete;

    // 允许移动
    TThreadSafeMap(TThreadSafeMap&& Other) noexcept
    {
        std::lock_guard<std::mutex> Lock(Other.Mutex);
        Map = std::move(Other.Map);
        MySize.store(Other.MySize.load());
    }

    TThreadSafeMap& operator=(TThreadSafeMap&& Other) noexcept
    {
        if (this != &Other)
        {
            std::lock_guard<std::mutex> Lock1(Mutex, std::adopt_lock);
            std::lock_guard<std::mutex> Lock2(Other.Mutex, std::adopt_lock);
            std::lock(Mutex, Other.Mutex);
            Map = std::move(Other.Map);
            MySize.store(Other.MySize.load());
        }
        return *this;
    }

    void Insert(const Key& InKey, const Value& InValue)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Map[InKey] = InValue;
        MySize.store(Map.size(), std::memory_order_release);
    }

    void Insert(const Key& InKey, Value&& InValue)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Map[InKey] = std::move(InValue);
        MySize.store(Map.size(), std::memory_order_release);
    }

    template <typename... Args>
    void Emplace(const Key& InKey, Args&&... Args_)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Map.emplace(std::piecewise_construct, std::forward_as_tuple(InKey), std::forward_as_tuple(std::forward<Args>(Args_)...));
        MySize.store(Map.size(), std::memory_order_release);
    }

    bool TryGet(const Key& InKey, Value& OutValue) const
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        auto It = Map.find(InKey);
        if (It != Map.end())
        {
            OutValue = It->second;
            return true;
        }
        return false;
    }

    Value* Find(const Key& InKey)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        auto It = Map.find(InKey);
        if (It != Map.end())
        {
            return &It->second;
        }
        return nullptr;
    }

    const Value* Find(const Key& InKey) const
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        auto It = Map.find(InKey);
        if (It != Map.end())
        {
            return &It->second;
        }
        return nullptr;
    }

    bool Contains(const Key& InKey) const
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        return Map.find(InKey) != Map.end();
    }

    bool Remove(const Key& InKey)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        auto It = Map.find(InKey);
        if (It != Map.end())
        {
            Map.erase(It);
            MySize.store(Map.size(), std::memory_order_release);
            return true;
        }
        return false;
    }

    void Clear()
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Map.clear();
        MySize.store(0, std::memory_order_release);
    }

    size_t Size() const
    {
        return MySize.load(std::memory_order_acquire);
    }

    bool IsEmpty() const
    {
        return MySize.load(std::memory_order_acquire) == 0;
    }

private:
    mutable std::mutex Mutex;
    std::unordered_map<Key, Value> Map;
    std::atomic<size_t> MySize{0};
};

// 为TSharedPtr提供hash特化
namespace std
{
    template <typename T>
    struct hash<TSharedPtr<T>>
    {
        size_t operator()(const TSharedPtr<T>& Ptr) const noexcept
        {
            // 使用指针地址进行hash
            return std::hash<const void*>{}(static_cast<const void*>(Ptr.Get()));
        }
    };
}

