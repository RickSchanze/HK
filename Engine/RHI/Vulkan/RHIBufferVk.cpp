//
// Created by Admin on 2025/12/15.
//

#include "GfxDeviceVk.h"
#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"
#include "Core/Utility/Macros.h"
#include "RHI/RHIBuffer.h"
#include "RHI/RHIHandle.h"
#include <stdexcept>

FRHIBuffer FGfxDeviceVk::CreateBuffer(const FRHIBufferDesc& BufferCreateInfo)
{
    if (!Device)
    {
        HK_LOG_FATAL(ELogcat::RHI, "Vulkan Device未创建，请先调用Initialize()");
        throw std::runtime_error("Vulkan Device未创建");
    }

    if (BufferCreateInfo.Size == 0)
    {
        HK_LOG_FATAL(ELogcat::RHI, "缓冲区大小不能为0");
        throw std::runtime_error("缓冲区大小不能为0");
    }

    // 转换使用标志
    const vk::BufferUsageFlags UsageFlags = ConvertBufferUsage(BufferCreateInfo.Usage);
    const vk::MemoryPropertyFlags MemoryFlags = ConvertMemoryProperty(BufferCreateInfo.MemoryProperty);

    // 创建 Buffer
    vk::BufferCreateInfo BufferInfo;
    BufferInfo.size = BufferCreateInfo.Size;
    BufferInfo.usage = UsageFlags;
    BufferInfo.sharingMode = vk::SharingMode::eExclusive;

    vk::Buffer VkBuffer;
    try
    {
        VkBuffer = Device.createBuffer(BufferInfo);
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建Vulkan Buffer失败: {}", e.what());
        throw std::runtime_error((FString("创建Vulkan Buffer失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建Vulkan Buffer失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建Vulkan Buffer失败: 未知异常");
        throw std::runtime_error("创建Vulkan Buffer失败: 未知异常");
    }

    // 获取内存需求
    const vk::MemoryRequirements MemRequirements = Device.getBufferMemoryRequirements(VkBuffer);

    // 分配内存
    vk::MemoryAllocateInfo AllocInfo;
    AllocInfo.allocationSize = MemRequirements.size;
    AllocInfo.memoryTypeIndex = FindMemoryType(MemRequirements.memoryTypeBits, MemoryFlags);

    vk::DeviceMemory DeviceMemory;
    try
    {
        DeviceMemory = Device.allocateMemory(AllocInfo);
    }
    catch (const vk::SystemError& e)
    {
        Device.destroyBuffer(VkBuffer);
        HK_LOG_FATAL(ELogcat::RHI, "分配Vulkan内存失败: {}", e.what());
        throw std::runtime_error((FString("分配Vulkan内存失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        Device.destroyBuffer(VkBuffer);
        HK_LOG_FATAL(ELogcat::RHI, "分配Vulkan内存失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        Device.destroyBuffer(VkBuffer);
        HK_LOG_FATAL(ELogcat::RHI, "分配Vulkan内存失败: 未知异常");
        throw std::runtime_error("分配Vulkan内存失败: 未知异常");
    }

    // 绑定内存到 Buffer
    try
    {
        Device.bindBufferMemory(VkBuffer, DeviceMemory, 0);
    }
    catch (const vk::SystemError& e)
    {
        Device.freeMemory(DeviceMemory);
        Device.destroyBuffer(VkBuffer);
        HK_LOG_FATAL(ELogcat::RHI, "绑定Buffer内存失败: {}", e.what());
        throw std::runtime_error((FString("绑定Buffer内存失败: ") + FString(e.what())).CStr());
    }
    catch (const std::exception& e)
    {
        Device.freeMemory(DeviceMemory);
        Device.destroyBuffer(VkBuffer);
        HK_LOG_FATAL(ELogcat::RHI, "绑定Buffer内存失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        Device.freeMemory(DeviceMemory);
        Device.destroyBuffer(VkBuffer);
        HK_LOG_FATAL(ELogcat::RHI, "绑定Buffer内存失败: 未知异常");
        throw std::runtime_error("绑定Buffer内存失败: 未知异常");
    }

    // 设置 DebugName
    if (!BufferCreateInfo.DebugName.IsEmpty())
    {
        SetDebugName(VkBuffer, vk::ObjectType::eBuffer, FStringView(BufferCreateInfo.DebugName));
        SetDebugName(DeviceMemory, vk::ObjectType::eDeviceMemory, FStringView(BufferCreateInfo.DebugName));
    }

    // 创建 RHI Handle（存储 Buffer 和 Memory 的指针）
    struct FBufferData
    {
        vk::Buffer Buffer;
        vk::DeviceMemory Memory;
    };
    auto* BufferData = new FBufferData{VkBuffer, DeviceMemory};

    auto& HandleManager = FRHIHandleManager::GetRef();
    const FString DebugNameStr = BufferCreateInfo.DebugName.IsEmpty() ? FString("Buffer") : BufferCreateInfo.DebugName;
    const FRHIHandle BufferHandle = HandleManager.CreateRHIHandle(DebugNameStr.CStr(), BufferData);

    // 创建并返回 FRHIBuffer
    FRHIBuffer Buffer;
    Buffer.Handle = BufferHandle;
    Buffer.Size = BufferCreateInfo.Size;
    Buffer.Usage = BufferCreateInfo.Usage;
    Buffer.MemoryProperty = BufferCreateInfo.MemoryProperty;

    HK_LOG_INFO(ELogcat::RHI, "Buffer创建成功: {} (大小: {} 字节)", DebugNameStr.CStr(), BufferCreateInfo.Size);
    return Buffer;
}

void FGfxDeviceVk::DestroyBuffer(FRHIBuffer& Buffer)
{
    if (!Buffer.IsValid())
    {
        HK_LOG_WARN(ELogcat::RHI, "尝试销毁无效的Buffer");
        return;
    }

    if (!Device)
    {
        HK_LOG_WARN(ELogcat::RHI, "Vulkan Device未创建，无法销毁Buffer");
        return;
    }

    try
    {
        // 如果 Buffer 已映射，先取消映射
        if (Buffer.IsMapped())
        {
            UnmapBuffer(Buffer);
        }

        // 获取 Buffer 数据
        struct FBufferData
        {
            vk::Buffer Buffer;
            vk::DeviceMemory Memory;
        };

        if (const FBufferData* BufferData = Buffer.GetHandle().Cast<FBufferData*>())
        {
            // 销毁 Buffer 和释放内存
            try
            {
                Device.destroyBuffer(BufferData->Buffer);
            }
            catch (const vk::SystemError& e)
            {
                HK_LOG_ERROR(ELogcat::RHI, "销毁Buffer失败: {}", e.what());
            }
            catch (const std::exception& e)
            {
                HK_LOG_ERROR(ELogcat::RHI, "销毁Buffer失败: {}", e.what());
            }
            catch (...)
            {
                HK_LOG_ERROR(ELogcat::RHI, "销毁Buffer失败: 未知异常");
            }

            try
            {
                Device.freeMemory(BufferData->Memory);
            }
            catch (const vk::SystemError& e)
            {
                HK_LOG_ERROR(ELogcat::RHI, "释放Buffer内存失败: {}", e.what());
            }
            catch (const std::exception& e)
            {
                HK_LOG_ERROR(ELogcat::RHI, "释放Buffer内存失败: {}", e.what());
            }
            catch (...)
            {
                HK_LOG_ERROR(ELogcat::RHI, "释放Buffer内存失败: 未知异常");
            }

            // 删除 BufferData
            delete BufferData;
        }

        // 销毁 RHI Handle
        auto& HandleManager = FRHIHandleManager::GetRef();
        HandleManager.DestroyRHIHandle(Buffer.GetHandle());

        // 清空 Buffer
        Buffer = FRHIBuffer();

        HK_LOG_INFO(ELogcat::RHI, "Buffer已销毁");
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::RHI, "销毁Buffer时发生异常: {}", e.what());
    }
    catch (...)
    {
        HK_LOG_ERROR(ELogcat::RHI, "销毁Buffer时发生未知异常");
    }
}

void* FGfxDeviceVk::MapBuffer(FRHIBuffer& Buffer, UInt64 Offset, UInt64 Size)
{
    if (!Buffer.IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "尝试映射无效的Buffer");
        return nullptr;
    }

    if (!Device)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Vulkan Device未创建，无法映射Buffer");
        return nullptr;
    }

    if (Buffer.IsMapped())
    {
        HK_LOG_WARN(ELogcat::RHI, "Buffer已经映射，返回现有映射指针");
        return Buffer.GetMappedPtr();
    }

    // 检查内存属性
    if (!HasFlag(Buffer.GetMemoryProperty(), ERHIBufferMemoryProperty::HostVisible))
    {
        HK_LOG_ERROR(ELogcat::RHI, "Buffer必须具有HostVisible内存属性才能映射");
        return nullptr;
    }

    // 计算映射大小
    UInt64 MapSize = Size;
    if (MapSize == 0)
    {
        MapSize = Buffer.GetSize() - Offset;
    }

    if (Offset + MapSize > Buffer.GetSize())
    {
        HK_LOG_ERROR(ELogcat::RHI, "映射范围超出Buffer大小");
        return nullptr;
    }

    // 获取 Buffer 数据
    struct FBufferData
    {
        vk::Buffer Buffer;
        vk::DeviceMemory Memory;
    };

    const FBufferData* BufferData = Buffer.GetHandle().Cast<FBufferData*>();
    if (!BufferData)
    {
        HK_LOG_ERROR(ELogcat::RHI, "无法获取Buffer数据");
        return nullptr;
    }

    // 映射内存
    void* MappedPtr = nullptr;
    try
    {
        MappedPtr = Device.mapMemory(BufferData->Memory, Offset, MapSize);
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_ERROR(ELogcat::RHI, "映射Buffer内存失败: {}", e.what());
        return nullptr;
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::RHI, "映射Buffer内存失败: {}", e.what());
        return nullptr;
    }
    catch (...)
    {
        HK_LOG_ERROR(ELogcat::RHI, "映射Buffer内存失败: 未知异常");
        return nullptr;
    }

    // 更新 Buffer 的映射指针
    Buffer.MappedPtr = MappedPtr;

    HK_LOG_INFO(ELogcat::RHI, "Buffer映射成功: Offset={}, Size={}", Offset, MapSize);
    return MappedPtr;
}

void FGfxDeviceVk::UnmapBuffer(FRHIBuffer& Buffer)
{
    if (!Buffer.IsValid())
    {
        HK_LOG_WARN(ELogcat::RHI, "尝试取消映射无效的Buffer");
        return;
    }

    if (!Buffer.IsMapped())
    {
        HK_LOG_WARN(ELogcat::RHI, "Buffer未映射，无需取消映射");
        return;
    }

    if (!Device)
    {
        HK_LOG_WARN(ELogcat::RHI, "Vulkan Device未创建，无法取消映射Buffer");
        Buffer.MappedPtr = nullptr;
        return;
    }

    // 获取 Buffer 数据
    struct FBufferData
    {
        vk::Buffer Buffer;
        vk::DeviceMemory Memory;
    };

    if (const FBufferData* BufferData = Buffer.GetHandle().Cast<FBufferData*>())
    {
        try
        {
            Device.unmapMemory(BufferData->Memory);
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_ERROR(ELogcat::RHI, "取消映射Buffer内存失败: {}", e.what());
        }
        catch (const std::exception& e)
        {
            HK_LOG_ERROR(ELogcat::RHI, "取消映射Buffer内存失败: {}", e.what());
        }
        catch (...)
        {
            HK_LOG_ERROR(ELogcat::RHI, "取消映射Buffer内存失败: 未知异常");
        }
    }

    // 清空映射指针
    Buffer.MappedPtr = nullptr;

    HK_LOG_INFO(ELogcat::RHI, "Buffer取消映射成功");
}

vk::BufferUsageFlags FGfxDeviceVk::ConvertBufferUsage(const ERHIBufferUsage Usage)
{
    vk::BufferUsageFlags Flags = {};

    if (HasFlag(Usage, ERHIBufferUsage::TransferSrc))
    {
        Flags |= vk::BufferUsageFlagBits::eTransferSrc;
    }
    if (HasFlag(Usage, ERHIBufferUsage::TransferDst))
    {
        Flags |= vk::BufferUsageFlagBits::eTransferDst;
    }
    if (HasFlag(Usage, ERHIBufferUsage::UniformTexelBuffer))
    {
        Flags |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
    }
    if (HasFlag(Usage, ERHIBufferUsage::StorageTexelBuffer))
    {
        Flags |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
    }
    if (HasFlag(Usage, ERHIBufferUsage::UniformBuffer))
    {
        Flags |= vk::BufferUsageFlagBits::eUniformBuffer;
    }
    if (HasFlag(Usage, ERHIBufferUsage::StorageBuffer))
    {
        Flags |= vk::BufferUsageFlagBits::eStorageBuffer;
    }
    if (HasFlag(Usage, ERHIBufferUsage::IndexBuffer))
    {
        Flags |= vk::BufferUsageFlagBits::eIndexBuffer;
    }
    if (HasFlag(Usage, ERHIBufferUsage::VertexBuffer))
    {
        Flags |= vk::BufferUsageFlagBits::eVertexBuffer;
    }
    if (HasFlag(Usage, ERHIBufferUsage::IndirectBuffer))
    {
        Flags |= vk::BufferUsageFlagBits::eIndirectBuffer;
    }

    return Flags;
}

vk::MemoryPropertyFlags FGfxDeviceVk::ConvertMemoryProperty(const ERHIBufferMemoryProperty MemoryProperty)
{
    vk::MemoryPropertyFlags Flags = {};

    if (HasFlag(MemoryProperty, ERHIBufferMemoryProperty::DeviceLocal))
    {
        Flags |= vk::MemoryPropertyFlagBits::eDeviceLocal;
    }
    if (HasFlag(MemoryProperty, ERHIBufferMemoryProperty::HostVisible))
    {
        Flags |= vk::MemoryPropertyFlagBits::eHostVisible;
    }
    if (HasFlag(MemoryProperty, ERHIBufferMemoryProperty::HostCoherent))
    {
        Flags |= vk::MemoryPropertyFlagBits::eHostCoherent;
    }
    if (HasFlag(MemoryProperty, ERHIBufferMemoryProperty::HostCached))
    {
        Flags |= vk::MemoryPropertyFlagBits::eHostCached;
    }

    return Flags;
}

