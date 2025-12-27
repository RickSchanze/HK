#pragma once

#include <atomic>
#include <mutex>
#include <queue>

template <typename T>
class TThreadSafeQueue
{
public:
    TThreadSafeQueue() = default;
    ~TThreadSafeQueue() = default;

    // 禁止拷贝
    TThreadSafeQueue(const TThreadSafeQueue&) = delete;
    TThreadSafeQueue& operator=(const TThreadSafeQueue&) = delete;

    // 允许移动
    TThreadSafeQueue(TThreadSafeQueue&& Other) noexcept
    {
        std::lock_guard<std::mutex> Lock(Other.Mutex);
        Queue = std::move(Other.Queue);
        MySize.store(Other.MySize.load());
    }

    TThreadSafeQueue& operator=(TThreadSafeQueue&& Other) noexcept
    {
        if (this != &Other)
        {
            std::lock_guard<std::mutex> Lock1(Mutex, std::adopt_lock);
            std::lock_guard<std::mutex> Lock2(Other.Mutex, std::adopt_lock);
            std::lock(Mutex, Other.Mutex);
            Queue = std::move(Other.Queue);
            MySize.store(Other.MySize.load());
        }
        return *this;
    }

    void Enqueue(const T& Value)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Queue.push(Value);
        MySize.fetch_add(1, std::memory_order_relaxed);
    }

    void Enqueue(T&& Value)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Queue.push(std::move(Value));
        MySize.fetch_add(1, std::memory_order_relaxed);
    }

    template <typename... Args>
    void Emplace(Args&&... Args_)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Queue.emplace(std::forward<Args>(Args_)...);
        MySize.fetch_add(1, std::memory_order_relaxed);
    }

    bool TryDequeue(T& OutValue)
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        if (Queue.empty())
        {
            return false;
        }
        OutValue = std::move(Queue.front());
        Queue.pop();
        MySize.fetch_sub(1, std::memory_order_relaxed);
        return true;
    }

    bool Dequeue(T& OutValue)
    {
        return TryDequeue(OutValue);
    }

    bool IsEmpty() const
    {
        return MySize.load(std::memory_order_acquire) == 0;
    }

    size_t Size() const
    {
        return MySize.load(std::memory_order_acquire);
    }

    void Clear()
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        while (!Queue.empty())
        {
            Queue.pop();
        }
        MySize.store(0, std::memory_order_release);
    }

private:
    mutable std::mutex Mutex;
    std::queue<T> Queue;
    std::atomic<size_t> MySize{0};
};

