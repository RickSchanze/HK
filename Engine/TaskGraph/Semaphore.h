#pragma once

#include "Core/Utility/Macros.h"

#ifdef HK_WINDOWS
typedef void* HANDLE;
#else
#error "Semaphore currently only supports Windows platform"
#endif

class FSemaphore
{
public:
    explicit FSemaphore(Int32 InitialCount = 0);
    ~FSemaphore();

    // 禁止拷贝
    FSemaphore(const FSemaphore&) = delete;
    FSemaphore& operator=(const FSemaphore&) = delete;

    // 允许移动
    FSemaphore(FSemaphore&& Other) noexcept : Semaphore(Other.Semaphore), Count(Other.Count)
    {
        Other.Semaphore = nullptr;
        Other.Count = 0;
    }

    FSemaphore& operator=(FSemaphore&& Other) noexcept;

    // 等待信号量（P操作）
    void Wait();

    // 尝试等待信号量（非阻塞）
    bool TryWait();

    // 释放信号量（V操作）
    void Signal();

    // 释放多个信号量
    void Signal(Int32 Count);

    // 获取当前计数（近似值，因为多线程环境下可能不准确）
    Int32 GetCount() const
    {
        return Count;
    }

private:
#ifdef HK_WINDOWS
    HANDLE Semaphore = nullptr;
#endif
    Int32 Count = 0;
};
