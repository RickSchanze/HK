//
// Created by Admin on 2025/12/21.
//

#include "GfxDeviceVk.h"

#include "Core/Container/Array.h"
#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "RHI/RHIHandle.h"

#pragma region DescriptorPool实现

FRHIDescriptorPool FGfxDeviceVk::CreateDescriptorPool(const FRHIDescriptorPoolDesc& PoolCreateInfo)
{
    try
    {
        // 转换描述符池大小数组
        TArray<vk::DescriptorPoolSize> VulkanPoolSizes;
        VulkanPoolSizes.Reserve(PoolCreateInfo.PoolSizes.Size());

        for (const auto& PoolSize : PoolCreateInfo.PoolSizes)
        {
            vk::DescriptorPoolSize VulkanPoolSize;
            VulkanPoolSize.type = ConvertDescriptorType(PoolSize.Type);
            VulkanPoolSize.descriptorCount = PoolSize.Count;
            VulkanPoolSizes.Add(VulkanPoolSize);
        }

        // 创建 Vulkan 描述符池创建信息
        vk::DescriptorPoolCreateInfo PoolInfo;
        PoolInfo.flags = ConvertDescriptorPoolCreateFlags(PoolCreateInfo.Flags);
        PoolInfo.maxSets = PoolCreateInfo.MaxSets;
        PoolInfo.poolSizeCount = static_cast<uint32_t>(VulkanPoolSizes.Size());
        PoolInfo.pPoolSizes = VulkanPoolSizes.Data();

        // 创建 Vulkan 描述符池
        vk::DescriptorPool VulkanPool;
        try
        {
            VulkanPool = Device.createDescriptorPool(PoolInfo);
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_FATAL(ELogcat::RHI, "创建 Vulkan 描述符池失败: {}", e.what());
            throw std::runtime_error((FString("创建 Vulkan 描述符池失败: ") + FString(e.what())).CStr());
        }

        // 设置调试名称
        if (!PoolCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanPool, vk::ObjectType::eDescriptorPool, PoolCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto& HandleManager = FRHIHandleManager::GetRef();
        FRHIHandle PoolHandle = HandleManager.CreateRHIHandle(
            PoolCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkDescriptorPool>(VulkanPool)));

        // 创建 FRHIDescriptorPool 对象
        FRHIDescriptorPool DescriptorPool;
        DescriptorPool.Handle = PoolHandle;
        DescriptorPool.MaxSets = PoolCreateInfo.MaxSets;
        DescriptorPool.Flags = PoolCreateInfo.Flags;

        HK_LOG_INFO(ELogcat::RHI, "描述符池创建成功: MaxSets={}, PoolSizes={}", PoolCreateInfo.MaxSets,
                    PoolCreateInfo.PoolSizes.Size());

        return DescriptorPool;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建描述符池失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建描述符池失败: 未知异常");
        throw std::runtime_error("创建描述符池失败: 未知异常");
    }
}

void FGfxDeviceVk::DestroyDescriptorPool(FRHIDescriptorPool& DescriptorPool)
{
    if (!DescriptorPool.IsValid())
    {
        return;
    }

    // 获取 Vulkan 描述符池句柄
    const auto VulkanPool = vk::DescriptorPool(DescriptorPool.Handle.Cast<VkDescriptorPool>());

    // 销毁 Vulkan 描述符池（会自动释放所有从中分配的描述符集）
    if (Device && VulkanPool)
    {
        Device.destroyDescriptorPool(VulkanPool);
    }

    // 销毁 RHI 句柄
    auto& HandleManager = FRHIHandleManager::GetRef();
    HandleManager.DestroyRHIHandle(DescriptorPool.Handle);

    // 重置描述符池对象
    DescriptorPool = FRHIDescriptorPool();

    HK_LOG_INFO(ELogcat::RHI, "描述符池已销毁");
}

#pragma endregion

#pragma region DescriptorSet实现

FRHIDescriptorSet FGfxDeviceVk::AllocateDescriptorSet(const FRHIDescriptorPool& Pool,
                                                      const FRHIDescriptorSetDesc& SetCreateInfo)
{
    HK_ASSERT_MSG_RAW(Pool.IsValid(), "无效的描述符池句柄");
    HK_ASSERT_MSG_RAW(SetCreateInfo.Layout != nullptr, "描述符集布局不能为空");

    try
    {
        // 获取 Vulkan 描述符池和布局
        const auto VulkanPool = vk::DescriptorPool(Pool.Handle.Cast<VkDescriptorPool>());
        const auto VulkanLayout = vk::DescriptorSetLayout(static_cast<VkDescriptorSetLayout>(SetCreateInfo.Layout));

        // 创建 Vulkan 描述符集分配信息
        vk::DescriptorSetAllocateInfo AllocInfo;
        AllocInfo.descriptorPool = VulkanPool;
        AllocInfo.descriptorSetCount = 1;
        AllocInfo.pSetLayouts = &VulkanLayout;

        // 分配 Vulkan 描述符集
        TArray<vk::DescriptorSet> VulkanSets;
        try
        {
            const auto SetsVector = Device.allocateDescriptorSets(AllocInfo);
            VulkanSets.Reserve(SetsVector.size());
            for (const auto& Set : SetsVector)
            {
                VulkanSets.Add(Set);
            }
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_FATAL(ELogcat::RHI, "分配 Vulkan 描述符集失败: {}", e.what());
            throw std::runtime_error((FString("分配 Vulkan 描述符集失败: ") + FString(e.what())).CStr());
        }

        if (VulkanSets.IsEmpty())
        {
            HK_LOG_FATAL(ELogcat::RHI, "分配的描述符集为空");
            throw std::runtime_error("分配的描述符集为空");
        }

        const vk::DescriptorSet VulkanSet = VulkanSets[0];

        // 设置调试名称
        if (!SetCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanSet, vk::ObjectType::eDescriptorSet, SetCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto& HandleManager = FRHIHandleManager::GetRef();
        const FRHIHandle SetHandle = HandleManager.CreateRHIHandle(
            SetCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkDescriptorSet>(VulkanSet)));

        // 创建 FRHIDescriptorSet 对象
        FRHIDescriptorSet DescriptorSet;
        DescriptorSet.Handle = SetHandle;
        DescriptorSet.Pool = &Pool; // 弱引用

        HK_LOG_INFO(ELogcat::RHI, "描述符集分配成功");

        return DescriptorSet;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "分配描述符集失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "分配描述符集失败: 未知异常");
        throw std::runtime_error("分配描述符集失败: 未知异常");
    }
}

void FGfxDeviceVk::FreeDescriptorSet(const FRHIDescriptorPool& Pool, FRHIDescriptorSet& DescriptorSet)
{
    if (!DescriptorSet.IsValid())
    {
        return;
    }

    HK_ASSERT_MSG_RAW(Pool.IsValid(), "无效的描述符池句柄");

    // 获取 Vulkan 描述符池和描述符集句柄
    const auto VulkanPool = vk::DescriptorPool(Pool.Handle.Cast<VkDescriptorPool>());
    const auto VulkanSet = vk::DescriptorSet(DescriptorSet.Handle.Cast<VkDescriptorSet>());

    // 释放 Vulkan 描述符集
    if (Device && VulkanPool && VulkanSet)
    {
        try
        {
            Device.freeDescriptorSets(VulkanPool, {VulkanSet});
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_ERROR(ELogcat::RHI, "释放描述符集失败: {}", e.what());
            // 继续执行，尝试清理 RHI 句柄
        }
    }

    // 销毁 RHI 句柄
    auto& HandleManager = FRHIHandleManager::GetRef();
    HandleManager.DestroyRHIHandle(DescriptorSet.Handle);

    // 重置描述符集对象
    DescriptorSet = FRHIDescriptorSet();

    HK_LOG_INFO(ELogcat::RHI, "描述符集已释放");
}

#pragma endregion

#pragma region 转换函数实现

vk::DescriptorType FGfxDeviceVk::ConvertDescriptorType(ERHIDescriptorType Type)
{
    switch (Type)
    {
        case ERHIDescriptorType::Sampler:
            return vk::DescriptorType::eSampler;
        case ERHIDescriptorType::CombinedImageSampler:
            return vk::DescriptorType::eCombinedImageSampler;
        case ERHIDescriptorType::SampledImage:
            return vk::DescriptorType::eSampledImage;
        case ERHIDescriptorType::StorageImage:
            return vk::DescriptorType::eStorageImage;
        case ERHIDescriptorType::UniformTexelBuffer:
            return vk::DescriptorType::eUniformTexelBuffer;
        case ERHIDescriptorType::StorageTexelBuffer:
            return vk::DescriptorType::eStorageTexelBuffer;
        case ERHIDescriptorType::UniformBuffer:
            return vk::DescriptorType::eUniformBuffer;
        case ERHIDescriptorType::StorageBuffer:
            return vk::DescriptorType::eStorageBuffer;
        case ERHIDescriptorType::UniformBufferDynamic:
            return vk::DescriptorType::eUniformBufferDynamic;
        case ERHIDescriptorType::StorageBufferDynamic:
            return vk::DescriptorType::eStorageBufferDynamic;
        case ERHIDescriptorType::InputAttachment:
            return vk::DescriptorType::eInputAttachment;
        case ERHIDescriptorType::AccelerationStructureKHR:
            return vk::DescriptorType::eAccelerationStructureKHR;
        case ERHIDescriptorType::AccelerationStructureNV:
            return vk::DescriptorType::eAccelerationStructureNV;
        case ERHIDescriptorType::MutableVALVE:
            return vk::DescriptorType::eMutableVALVE;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的描述符类型: {}", static_cast<UInt32>(Type));
            return vk::DescriptorType::eUniformBuffer;
    }
}

vk::DescriptorPoolCreateFlags FGfxDeviceVk::ConvertDescriptorPoolCreateFlags(ERHIDescriptorPoolCreateFlag Flags)
{
    vk::DescriptorPoolCreateFlags VulkanFlags = {};

    if (HasFlag(Flags, ERHIDescriptorPoolCreateFlag::FreeDescriptorSet))
    {
        VulkanFlags |= vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    }
    if (HasFlag(Flags, ERHIDescriptorPoolCreateFlag::UpdateAfterBind))
    {
        VulkanFlags |= vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
    }

    return VulkanFlags;
}

#pragma endregion

void FGfxDeviceVk::SetDebugName(vk::DescriptorPool ObjectHandle, vk::ObjectType ObjectType,
                                const FStringView& Name) const
{
    if (!Device || Name.IsEmpty() || !bDebugUtilsExtensionAvailable)
    {
        return;
    }

    try
    {
        const FString NameStr(Name.Data(), Name.Size());

        const auto vkSetDebugUtilsObjectNameEXT =
            reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(Device.getProcAddr("vkSetDebugUtilsObjectNameEXT"));

        if (vkSetDebugUtilsObjectNameEXT)
        {
            VkDebugUtilsObjectNameInfoEXT VkNameInfo{};
            VkNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            VkNameInfo.objectType = static_cast<VkObjectType>(ObjectType);
            VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkDescriptorPool>(ObjectHandle));
            VkNameInfo.pObjectName = NameStr.CStr();

            vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
        }
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置DescriptorPool DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}

void FGfxDeviceVk::SetDebugName(vk::DescriptorSet ObjectHandle, vk::ObjectType ObjectType,
                                const FStringView& Name) const
{
    if (!Device || Name.IsEmpty() || !bDebugUtilsExtensionAvailable)
    {
        return;
    }

    try
    {
        const FString NameStr(Name.Data(), Name.Size());

        const auto vkSetDebugUtilsObjectNameEXT =
            reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(Device.getProcAddr("vkSetDebugUtilsObjectNameEXT"));

        if (vkSetDebugUtilsObjectNameEXT)
        {
            VkDebugUtilsObjectNameInfoEXT VkNameInfo{};
            VkNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            VkNameInfo.objectType = static_cast<VkObjectType>(ObjectType);
            VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkDescriptorSet>(ObjectHandle));
            VkNameInfo.pObjectName = NameStr.CStr();

            vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
        }
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置DescriptorSet DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}
