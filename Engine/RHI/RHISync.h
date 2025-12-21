#pragma once

#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "RHIHandle.h"

enum class ERHISemaphoreType : UInt32
{
    Binary = 0,    // 二进制信号量（默认）
    Timeline = 1,  // 时间线信号量（需要Vulkan 1.2+）
};

struct FRHISemaphoreDesc
{
    ERHISemaphoreType Type = ERHISemaphoreType::Binary; // 信号量类型
    UInt64 InitialValue = 0;                            // 初始值（仅用于时间线信号量）
    FString DebugName;                                  // 调试名称
};

// 信号量类
// 信号量用于GPU命令之间的同步，通常用于队列提交之间的同步
class FRHISemaphore
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 Semaphore（无效）
    FRHISemaphore() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::DestroySemaphore 销毁
    ~FRHISemaphore() = default;

    // 允许拷贝和移动
    FRHISemaphore(const FRHISemaphore& Other) = default;
    FRHISemaphore& operator=(const FRHISemaphore& Other) = default;
    FRHISemaphore(FRHISemaphore&& Other) noexcept = default;
    FRHISemaphore& operator=(FRHISemaphore&& Other) noexcept = default;

    // 检查是否有效
    bool IsValid() const
    {
        return Handle.IsValid();
    }

    // 获取底层句柄
    const FRHIHandle& GetHandle() const
    {
        return Handle;
    }

    FRHIHandle& GetHandle()
    {
        return Handle;
    }

    // 获取信号量类型
    ERHISemaphoreType GetType() const
    {
        return Type;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHISemaphore& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHISemaphore& Other) const
    {
        return Handle != Other.Handle;
    }

private:
    FRHIHandle Handle;
    ERHISemaphoreType Type = ERHISemaphoreType::Binary;
};

enum class ERHIFenceCreateFlag : UInt32
{
    None = 0,
    Signaled = 1 << 0, // 创建时已发出信号（初始状态为已发出信号）
};
HK_ENABLE_BITMASK_OPERATORS(ERHIFenceCreateFlag)

struct FRHIFenceDesc
{
    ERHIFenceCreateFlag Flags = ERHIFenceCreateFlag::None; // 创建标志
    FString DebugName;                                      // 调试名称
};

// 栅栏类
// 栅栏用于CPU和GPU之间的同步，可以等待GPU完成某些操作
class FRHIFence
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 Fence（无效）
    FRHIFence() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::DestroyFence 销毁
    ~FRHIFence() = default;

    // 允许拷贝和移动
    FRHIFence(const FRHIFence& Other) = default;
    FRHIFence& operator=(const FRHIFence& Other) = default;
    FRHIFence(FRHIFence&& Other) noexcept = default;
    FRHIFence& operator=(FRHIFence&& Other) noexcept = default;

    // 检查是否有效
    bool IsValid() const
    {
        return Handle.IsValid();
    }

    // 获取底层句柄
    const FRHIHandle& GetHandle() const
    {
        return Handle;
    }

    FRHIHandle& GetHandle()
    {
        return Handle;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHIFence& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHIFence& Other) const
    {
        return Handle != Other.Handle;
    }

private:
    FRHIHandle Handle;
};

