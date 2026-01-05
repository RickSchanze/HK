//
// Created by Admin on 2025/12/21.
//

#include "GfxDeviceVk.h"

#include <vulkan/vulkan.h>

#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "RHI/RHISync.h"
#include "RHI/RHIHandle.h"

#pragma region Semaphore实现

FRHISemaphore FGfxDeviceVk::CreateSemaphore(const FRHISemaphoreDesc& SemaphoreCreateInfo)
{
    try
    {
        // 创建 Vulkan 信号量创建信息
        vk::SemaphoreCreateInfo SemaphoreInfo;

        // 如果是时间线信号量，需要使用 SemaphoreTypeCreateInfo
        vk::SemaphoreTypeCreateInfo TypeInfo;
        if (SemaphoreCreateInfo.Type == ERHISemaphoreType::Timeline)
        {
            TypeInfo.semaphoreType = vk::SemaphoreType::eTimeline;
            TypeInfo.initialValue = SemaphoreCreateInfo.InitialValue;
            SemaphoreInfo.pNext = &TypeInfo;
        }

        // 创建 Vulkan 信号量
        vk::Semaphore VulkanSemaphore;
        try
        {
            VulkanSemaphore = Device.createSemaphore(SemaphoreInfo);
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_FATAL(ELogcat::RHI, "创建 Vulkan 信号量失败: {}", e.what());
            throw std::runtime_error((FString("创建 Vulkan 信号量失败: ") + FString(e.what())).CStr());
        }

        // 设置调试名称
        if (!SemaphoreCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanSemaphore, vk::ObjectType::eSemaphore, SemaphoreCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto& HandleManager = FRHIHandleManager::GetRef();
        const FRHIHandle SemaphoreHandle = HandleManager.CreateRHIHandle(
            SemaphoreCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkSemaphore>(VulkanSemaphore)));

        // 创建 FRHISemaphore 对象
        FRHISemaphore Semaphore;
        Semaphore.Handle = SemaphoreHandle;
        Semaphore.Type = SemaphoreCreateInfo.Type;

        HK_LOG_INFO(ELogcat::RHI, "信号量创建成功: Type={}", static_cast<UInt32>(SemaphoreCreateInfo.Type));

        return Semaphore;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建信号量失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建信号量失败: 未知异常");
        throw std::runtime_error("创建信号量失败: 未知异常");
    }
}

void FGfxDeviceVk::DestroySemaphore(FRHISemaphore& Semaphore)
{
    if (!Semaphore.IsValid())
    {
        return;
    }

    // 获取 Vulkan 信号量句柄
    const auto VulkanSemaphore = vk::Semaphore(Semaphore.Handle.Cast<VkSemaphore>());

    // 销毁 Vulkan 信号量
    if (Device && VulkanSemaphore)
    {
        Device.destroySemaphore(VulkanSemaphore);
    }

    // 销毁 RHI 句柄
    auto& HandleManager = FRHIHandleManager::GetRef();
    HandleManager.DestroyRHIHandle(Semaphore.Handle);

    // 重置信号量对象
    Semaphore = FRHISemaphore();

    HK_LOG_INFO(ELogcat::RHI, "信号量已销毁");
}

#pragma endregion

#pragma region Fence实现

FRHIFence FGfxDeviceVk::CreateFence(const FRHIFenceDesc& FenceCreateInfo)
{
    try
    {
        // 创建 Vulkan 栅栏创建信息
        vk::FenceCreateInfo FenceInfo;
        if (HasFlag(FenceCreateInfo.Flags, ERHIFenceCreateFlag::Signaled))
        {
            FenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        }

        // 创建 Vulkan 栅栏
        vk::Fence VulkanFence;
        try
        {
            VulkanFence = Device.createFence(FenceInfo);
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_FATAL(ELogcat::RHI, "创建 Vulkan 栅栏失败: {}", e.what());
            throw std::runtime_error((FString("创建 Vulkan 栅栏失败: ") + FString(e.what())).CStr());
        }

        // 设置调试名称
        if (!FenceCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanFence, vk::ObjectType::eFence, FenceCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto& HandleManager = FRHIHandleManager::GetRef();
        const FRHIHandle FenceHandle = HandleManager.CreateRHIHandle(
            FenceCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkFence>(VulkanFence)));

        // 创建 FRHIFence 对象
        FRHIFence Fence;
        Fence.Handle = FenceHandle;

        HK_LOG_INFO(ELogcat::RHI, "栅栏创建成功: Flags={}", static_cast<UInt32>(FenceCreateInfo.Flags));

        return Fence;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建栅栏失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建栅栏失败: 未知异常");
        throw std::runtime_error("创建栅栏失败: 未知异常");
    }
}

void FGfxDeviceVk::DestroyFence(FRHIFence& Fence)
{
    if (!Fence.IsValid())
    {
        return;
    }

    // 获取 Vulkan 栅栏句柄
    const auto VulkanFence = vk::Fence(Fence.Handle.Cast<VkFence>());

    // 销毁 Vulkan 栅栏
    if (Device && VulkanFence)
    {
        Device.destroyFence(VulkanFence);
    }

    // 销毁 RHI 句柄
    auto& HandleManager = FRHIHandleManager::GetRef();
    HandleManager.DestroyRHIHandle(Fence.Handle);

    // 重置栅栏对象
    Fence = FRHIFence();

    HK_LOG_INFO(ELogcat::RHI, "栅栏已销毁");
}

bool FGfxDeviceVk::WaitForFence(const FRHIFence& Fence, UInt64 Timeout)
{
    if (!Fence.IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Cannot wait for invalid fence");
        return false;
    }

    const auto VulkanFence = vk::Fence(Fence.GetHandle().Cast<VkFence>());
    if (!VulkanFence)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid Vulkan fence");
        return false;
    }

    try
    {
        vk::Result Result = Device.waitForFences(1, &VulkanFence, VK_TRUE, Timeout);
        if (Result == vk::Result::eSuccess)
        {
            return true;
        }
        else if (Result == vk::Result::eTimeout)
        {
            HK_LOG_WARN(ELogcat::RHI, "Fence wait timeout");
            return false;
        }
        else
        {
            HK_LOG_ERROR(ELogcat::RHI, "Failed to wait for fence: {}", static_cast<int>(Result));
            return false;
        }
    }
    catch (const vk::SystemError& Err)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Vulkan error while waiting for fence: {}", Err.what());
        return false;
    }
}

bool FGfxDeviceVk::IsFenceSignaled(const FRHIFence& Fence) const
{
    if (!Fence.IsValid())
    {
        return false;
    }

    const auto VulkanFence = vk::Fence(Fence.GetHandle().Cast<VkFence>());
    if (!VulkanFence)
    {
        return false;
    }

    try
    {
        vk::Result Result = Device.getFenceStatus(VulkanFence);
        return Result == vk::Result::eSuccess;
    }
    catch (const vk::SystemError& Err)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Vulkan error while checking fence status: {}", Err.what());
        return false;
    }
}

bool FGfxDeviceVk::ResetFence(const FRHIFence& Fence)
{
    if (!Fence.IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Cannot reset invalid fence");
        return false;
    }

    const auto VulkanFence = vk::Fence(Fence.GetHandle().Cast<VkFence>());
    if (!VulkanFence)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid Vulkan fence");
        return false;
    }

    try
    {
        vk::Result Result = Device.resetFences(1, &VulkanFence);
        if (Result != vk::Result::eSuccess)
        {
            HK_LOG_ERROR(ELogcat::RHI, "Failed to reset fence: {}", static_cast<int>(Result));
            return false;
        }
        return true;
    }
    catch (const vk::SystemError& Err)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Vulkan error while resetting fence: {}", Err.what());
        return false;
    }
}

#pragma endregion

#pragma region SetDebugName实现

void FGfxDeviceVk::SetDebugName(vk::Semaphore ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const
{
    if (!Device || Name.IsEmpty() || !bDebugUtilsExtensionAvailable || !vkSetDebugUtilsObjectNameEXT)
    {
        return;
    }

    try
    {
        const FString NameStr(Name.Data(), Name.Size());

        VkDebugUtilsObjectNameInfoEXT VkNameInfo{};
        VkNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        VkNameInfo.objectType = static_cast<VkObjectType>(ObjectType);
        VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkSemaphore>(ObjectHandle));
        VkNameInfo.pObjectName = NameStr.CStr();

        vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置Semaphore DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}

void FGfxDeviceVk::SetDebugName(vk::Fence ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const
{
    if (!Device || Name.IsEmpty() || !bDebugUtilsExtensionAvailable || !vkSetDebugUtilsObjectNameEXT)
    {
        return;
    }

    try
    {
        const FString NameStr(Name.Data(), Name.Size());

        VkDebugUtilsObjectNameInfoEXT VkNameInfo{};
        VkNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        VkNameInfo.objectType = static_cast<VkObjectType>(ObjectType);
        VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkFence>(ObjectHandle));
        VkNameInfo.pObjectName = NameStr.CStr();

        vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置Fence DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}

#pragma endregion

