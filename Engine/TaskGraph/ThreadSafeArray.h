#pragma once

#include "Core/Container/Array.h"

#include <atomic>
#include <mutex>

template <typename T>
class TThreadSafeArray
{
public:
    TThreadSafeArray() = default;
    ~TThreadSafeArray() = default;

    // 禁止拷贝
    TThreadSafeArray(const TThreadSafeArray&) = delete;
    TThreadSafeArray& operator=(const TThreadSafeArray&) = delete;

    // 允许移动
    TThreadSafeArray(TThreadSafeArray&& Other) noexcept
    {
        std::lock_guard<std::mutex> Lock(Other.Mutex);
        Vector = std::move(Other.Vector);
        MySize.store(Other.MySize.load());
    }

    TThreadSafeArray& operator=(TThreadSafeArray&& Other) noexcept
    {
        if (this != &Other)
        {
            std::lock_guard<std::mutex> Lock1(Mutex, std::adopt_lock);
            std::lock_guard<std::mutex> Lock2(Other.Mutex, std::adopt_lock);
            std::lock(Mutex, Other.Mutex);
            Vector = std::move(Other.Vector);
            MySize.store(Other.MySize.load());
        }
        return *this;
    }

    void PushBack(const T& Value)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Vector.Add(Value);
        MySize.store(Vector.Size(), std::memory_order_release);
    }

    void PushBack(T&& Value)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Vector.Add(std::move(Value));
        MySize.store(Vector.Size(), std::memory_order_release);
    }

    template <typename... Args>
    void EmplaceBack(Args&&... Args_)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Vector.Emplace(std::forward<Args>(Args_)...);
        MySize.store(Vector.Size(), std::memory_order_release);
    }

    void Clear()
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Vector.Clear();
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

    // 获取所有元素的快照（线程安全）
    TArray<T> Snapshot() const
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        return TArray<T>(Vector.begin(), Vector.end());
    }

private:
    mutable std::mutex Mutex;
    TArray<T> Vector;
    std::atomic<size_t> MySize{0};
};

