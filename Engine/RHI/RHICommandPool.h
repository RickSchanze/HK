#pragma once

#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "Core/Utility/HashUtility.h"
#include "RHIHandle.h"

// 命令池创建标志
enum class ERHICommandPoolCreateFlag : UInt32
{
    None                = 0,
    Transient           = 1 << 0, // 命令缓冲区是临时的（频繁重置）
    ResetCommandBuffer  = 1 << 1, // 允许单独重置命令缓冲区
};
HK_ENABLE_BITMASK_OPERATORS(ERHICommandPoolCreateFlag)

// 命令池描述
struct FRHICommandPoolDesc
{
    ERHICommandPoolCreateFlag Flags = ERHICommandPoolCreateFlag::None; // 创建标志
    UInt32                    QueueFamilyIndex = 0;                    // 队列族索引
    FString                   DebugName;                               // 调试名称

    UInt64 GetHashCode() const
    {
        return FHashUtility::CombineHashes(std::hash<UInt32>{}(static_cast<UInt32>(Flags)),
                                           std::hash<UInt32>{}(QueueFamilyIndex));
    }
};

// 命令池类
class FRHICommandPool
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 CommandPool（无效）
    FRHICommandPool() = default;

    // 析构函数：不自动销毁资源，必须通过 FGfxDevice::DestroyCommandPool 销毁
    ~FRHICommandPool() = default;

    // 允许拷贝和移动
    FRHICommandPool(const FRHICommandPool& Other)            = default;
    FRHICommandPool& operator=(const FRHICommandPool& Other) = default;
    FRHICommandPool(FRHICommandPool&& Other) noexcept       = default;
    FRHICommandPool& operator=(FRHICommandPool&& Other) noexcept = default;

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

    // 获取队列族索引
    UInt32 GetQueueFamilyIndex() const
    {
        return QueueFamilyIndex;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHICommandPool& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHICommandPool& Other) const
    {
        return Handle != Other.Handle;
    }

    UInt64 GetHashCode() const
    {
        return Handle.GetHashCode();
    }

private:
    FRHIHandle Handle;
    UInt32     QueueFamilyIndex = 0;
};

