#pragma once

#include "Core/String/StringView.h"
#include "Core/Utility/Macros.h"
#include "RHIHandle.h"

enum class EBufferUsage : UInt32
{
    None = 0,
    TransferSrc = 1 << 0,        // 可以作为传输源
    TransferDst = 1 << 1,        // 可以作为传输目标
    UniformTexelBuffer = 1 << 2, // 统一纹理缓冲区
    StorageTexelBuffer = 1 << 3, // 存储纹理缓冲区
    UniformBuffer = 1 << 4,      // 统一缓冲区
    StorageBuffer = 1 << 5,      // 存储缓冲区
    IndexBuffer = 1 << 6,        // 索引缓冲区
    VertexBuffer = 1 << 7,       // 顶点缓冲区
    IndirectBuffer = 1 << 8,     // 间接缓冲区
};
HK_ENABLE_BITMASK_OPERATORS(EBufferUsage)

enum class EBufferMemoryProperty : UInt32
{
    None = 0,
    DeviceLocal = 1 << 0,  // 设备本地内存（GPU 访问快）
    HostVisible = 1 << 1,  // 主机可见（CPU 可访问）
    HostCoherent = 1 << 2, // 主机一致（CPU/GPU 自动同步）
    HostCached = 1 << 3,   // 主机缓存（CPU 读取快）
};
HK_ENABLE_BITMASK_OPERATORS(EBufferMemoryProperty)

struct FRHIBufferCreateInfo
{
    UInt64 Size = 0;                                                           // 缓冲区大小（字节）
    EBufferUsage Usage = EBufferUsage::None;                                   // 使用标志
    EBufferMemoryProperty MemoryProperty = EBufferMemoryProperty::DeviceLocal; // 内存属性
    FStringView DebugName;                                                     // 调试名称
};

// 简单的 Buffer 值类型，只包含一个 Handle
class FRHIBuffer
{
    friend class FGfxDevice;

public:
    // 默认构造：创建空的 Buffer（无效）
    FRHIBuffer() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::DestroyBuffer 销毁
    ~FRHIBuffer() = default;

    // 允许拷贝和移动
    FRHIBuffer(const FRHIBuffer& Other) = default;
    FRHIBuffer& operator=(const FRHIBuffer& Other) = default;
    FRHIBuffer(FRHIBuffer&& Other) noexcept = default;
    FRHIBuffer& operator=(FRHIBuffer&& Other) noexcept = default;

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

    // 获取缓冲区大小
    UInt64 GetSize() const
    {
        return Size;
    }

    // 获取使用标志
    EBufferUsage GetUsage() const
    {
        return Usage;
    }

    // 获取内存属性
    EBufferMemoryProperty GetMemoryProperty() const
    {
        return MemoryProperty;
    }

    // 映射内存（用于 CPU 访问）
    void* Map(UInt64 Offset = 0, UInt64 Size = 0);

    // 取消映射内存
    void Unmap();

    // 检查是否已映射
    bool IsMapped() const
    {
        return MappedPtr != nullptr;
    }

    // 获取映射的指针
    void* GetMappedPtr() const
    {
        return MappedPtr;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHIBuffer& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHIBuffer& Other) const
    {
        return Handle != Other.Handle;
    }

private:
    // 销毁缓冲区资源（只能通过 FRHIDevice::DestroyBuffer 调用）
    void Destroy();

    FRHIHandle Handle;
    UInt64 Size = 0;
    EBufferUsage Usage = EBufferUsage::None;
    EBufferMemoryProperty MemoryProperty = EBufferMemoryProperty::None;
    void* MappedPtr = nullptr; // 映射的内存指针
};
