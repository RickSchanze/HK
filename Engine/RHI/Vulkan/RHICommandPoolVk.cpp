//
// Created by Admin on 2025/12/27.
//

#include "Core/Logging/Logger.h"
#include "Core/Utility/Macros.h"
#include "GfxDeviceVk.h"

// ============================================================================
// CommandPool 创建和销毁
// ============================================================================

FRHICommandPool FGfxDeviceVk::CreateCommandPool(const FRHICommandPoolDesc& PoolCreateInfo)
{
    FRHICommandPool Pool;

    // 转换创建标志
    vk::CommandPoolCreateFlags VkFlags = vk::CommandPoolCreateFlags();
    if (HasFlag(PoolCreateInfo.Flags, ERHICommandPoolCreateFlag::Transient))
    {
        VkFlags |= vk::CommandPoolCreateFlagBits::eTransient;
    }
    if (HasFlag(PoolCreateInfo.Flags, ERHICommandPoolCreateFlag::ResetCommandBuffer))
    {
        VkFlags |= vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    }

    // 创建 Vulkan 命令池
    vk::CommandPoolCreateInfo VkCreateInfo(VkFlags, PoolCreateInfo.QueueFamilyIndex);
    vk::CommandPool           VkPool = Device.createCommandPool(VkCreateInfo);

    if (!VkPool)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Failed to create Vulkan command pool");
        return Pool;
    }

    // 创建 RHI Handle
    Pool.Handle           = FRHIHandleManager::GetRef().CreateRHIHandle(PoolCreateInfo.DebugName, VkPool);
    Pool.QueueFamilyIndex = PoolCreateInfo.QueueFamilyIndex;

    // 设置调试名称
    if (bDebugUtilsExtensionAvailable && !PoolCreateInfo.DebugName.IsEmpty())
    {
        SetDebugName(VkPool, vk::ObjectType::eCommandPool, PoolCreateInfo.DebugName);
    }

    return Pool;
}

void FGfxDeviceVk::DestroyCommandPool(FRHICommandPool& CommandPool)
{
    if (!CommandPool.IsValid())
    {
        return;
    }

    vk::CommandPool VkPool = CommandPool.Handle.Cast<vk::CommandPool>();
    Device.destroyCommandPool(VkPool);

    FRHIHandleManager::GetRef().DestroyRHIHandle(CommandPool.Handle);
    CommandPool.Handle           = FRHIHandle();
    CommandPool.QueueFamilyIndex = 0;
}
