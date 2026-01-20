//
// Created by Admin on 2025/12/21.
//

#include "GfxDeviceVk.h"

#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "RHI/RHIHandle.h"

#pragma region Sampler实现

FRHISampler FGfxDeviceVk::CreateSampler(const FRHISamplerDesc& SamplerCreateInfo)
{
    try
    {
        // 创建 Vulkan 采样器创建信息
        vk::SamplerCreateInfo SamplerInfo;
        SamplerInfo.magFilter               = ConvertFilter(SamplerCreateInfo.MagFilter);
        SamplerInfo.minFilter               = ConvertFilter(SamplerCreateInfo.MinFilter);
        SamplerInfo.mipmapMode              = ConvertSamplerMipmapMode(SamplerCreateInfo.MipmapMode);
        SamplerInfo.addressModeU            = ConvertSamplerAddressMode(SamplerCreateInfo.AddressModeU);
        SamplerInfo.addressModeV            = ConvertSamplerAddressMode(SamplerCreateInfo.AddressModeV);
        SamplerInfo.addressModeW            = ConvertSamplerAddressMode(SamplerCreateInfo.AddressModeW);
        SamplerInfo.mipLodBias              = SamplerCreateInfo.MipLodBias;
        SamplerInfo.anisotropyEnable        = SamplerCreateInfo.bAnisotropyEnable ? VK_TRUE : VK_FALSE;
        SamplerInfo.maxAnisotropy           = SamplerCreateInfo.MaxAnisotropy;
        SamplerInfo.compareEnable           = SamplerCreateInfo.bCompareEnable ? VK_TRUE : VK_FALSE;
        SamplerInfo.compareOp               = ConvertCompareOp(SamplerCreateInfo.CompareOp);
        SamplerInfo.minLod                  = SamplerCreateInfo.MinLod;
        SamplerInfo.maxLod                  = SamplerCreateInfo.MaxLod;
        SamplerInfo.borderColor             = ConvertSamplerBorderColor(SamplerCreateInfo.BorderColor);
        SamplerInfo.unnormalizedCoordinates = SamplerCreateInfo.bUnnormalizedCoordinates ? VK_TRUE : VK_FALSE;

        // 创建 Vulkan 采样器
        vk::Sampler VulkanSampler;
        try
        {
            VulkanSampler = Device.createSampler(SamplerInfo);
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_FATAL(ELogcat::RHI, "创建 Vulkan 采样器失败: {}", e.what());
            throw std::runtime_error((FString("创建 Vulkan 采样器失败: ") + FString(e.what())).CStr());
        }

        // 设置调试名称
        if (!SamplerCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanSampler, vk::ObjectType::eSampler, SamplerCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto&      HandleManager = FRHIHandleManager::GetRef();
        FRHIHandle SamplerHandle = HandleManager.CreateRHIHandle(
            SamplerCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkSampler>(VulkanSampler)));

        // 创建 FRHISampler 对象
        FRHISampler Sampler;
        Sampler.Handle       = SamplerHandle;
        Sampler.MagFilter    = SamplerCreateInfo.MagFilter;
        Sampler.MinFilter    = SamplerCreateInfo.MinFilter;
        Sampler.MipmapMode   = SamplerCreateInfo.MipmapMode;
        Sampler.AddressModeU = SamplerCreateInfo.AddressModeU;
        Sampler.AddressModeV = SamplerCreateInfo.AddressModeV;
        Sampler.AddressModeW = SamplerCreateInfo.AddressModeW;

        HK_LOG_INFO(ELogcat::RHI, "采样器创建成功: MagFilter={}, MinFilter={}, AddressMode={}",
                    static_cast<UInt32>(SamplerCreateInfo.MagFilter), static_cast<UInt32>(SamplerCreateInfo.MinFilter),
                    static_cast<UInt32>(SamplerCreateInfo.AddressModeU));

        return Sampler;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建采样器失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建采样器失败: 未知异常");
        throw std::runtime_error("创建采样器失败: 未知异常");
    }
}

void FGfxDeviceVk::DestroySampler(FRHISampler& Sampler)
{
    if (!Sampler.IsValid())
    {
        return;
    }

    // 获取 Vulkan 采样器句柄
    const auto VulkanSampler = vk::Sampler(Sampler.Handle.Cast<VkSampler>());

    // 销毁 Vulkan 采样器
    if (Device && VulkanSampler)
    {
        Device.destroySampler(VulkanSampler);
    }

    // 销毁 RHI 句柄
    auto& HandleManager = FRHIHandleManager::GetRef();
    HandleManager.DestroyRHIHandle(Sampler.Handle);

    // 重置采样器对象
    Sampler = FRHISampler();

    HK_LOG_INFO(ELogcat::RHI, "采样器已销毁");
}

#pragma endregion

#pragma region 转换函数实现

vk::Filter FGfxDeviceVk::ConvertFilter(ERHIFilter Filter)
{
    switch (Filter)
    {
        case ERHIFilter::Nearest:
            return vk::Filter::eNearest;
        case ERHIFilter::Linear:
            return vk::Filter::eLinear;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的过滤模式: {}", static_cast<UInt32>(Filter));
            return vk::Filter::eLinear;
    }
}

vk::SamplerAddressMode FGfxDeviceVk::ConvertSamplerAddressMode(ERHISamplerAddressMode AddressMode)
{
    switch (AddressMode)
    {
        case ERHISamplerAddressMode::Repeat:
            return vk::SamplerAddressMode::eRepeat;
        case ERHISamplerAddressMode::MirroredRepeat:
            return vk::SamplerAddressMode::eMirroredRepeat;
        case ERHISamplerAddressMode::ClampToEdge:
            return vk::SamplerAddressMode::eClampToEdge;
        case ERHISamplerAddressMode::ClampToBorder:
            return vk::SamplerAddressMode::eClampToBorder;
        case ERHISamplerAddressMode::MirrorClampToEdge:
            return vk::SamplerAddressMode::eMirrorClampToEdge;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的地址模式: {}", static_cast<UInt32>(AddressMode));
            return vk::SamplerAddressMode::eRepeat;
    }
}

vk::BorderColor FGfxDeviceVk::ConvertSamplerBorderColor(ERHISamplerBorderColor BorderColor)
{
    switch (BorderColor)
    {
        case ERHISamplerBorderColor::FloatTransparentBlack:
            return vk::BorderColor::eFloatTransparentBlack;
        case ERHISamplerBorderColor::IntTransparentBlack:
            return vk::BorderColor::eIntTransparentBlack;
        case ERHISamplerBorderColor::FloatOpaqueBlack:
            return vk::BorderColor::eFloatOpaqueBlack;
        case ERHISamplerBorderColor::IntOpaqueBlack:
            return vk::BorderColor::eIntOpaqueBlack;
        case ERHISamplerBorderColor::FloatOpaqueWhite:
            return vk::BorderColor::eFloatOpaqueWhite;
        case ERHISamplerBorderColor::IntOpaqueWhite:
            return vk::BorderColor::eIntOpaqueWhite;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的边界颜色: {}", static_cast<UInt32>(BorderColor));
            return vk::BorderColor::eFloatTransparentBlack;
    }
}

vk::SamplerMipmapMode FGfxDeviceVk::ConvertSamplerMipmapMode(ERHISamplerMipmapMode MipmapMode)
{
    switch (MipmapMode)
    {
        case ERHISamplerMipmapMode::Nearest:
            return vk::SamplerMipmapMode::eNearest;
        case ERHISamplerMipmapMode::Linear:
            return vk::SamplerMipmapMode::eLinear;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的 MIP 映射模式: {}", static_cast<UInt32>(MipmapMode));
            return vk::SamplerMipmapMode::eLinear;
    }
}

#pragma endregion

void FGfxDeviceVk::SetDebugName(vk::Sampler ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const
{
    if (!Device || Name.IsEmpty() || !bDebugUtilsExtensionAvailable || !vkSetDebugUtilsObjectNameEXT)
    {
        return;
    }

    try
    {
        const FString NameStr(Name.Data(), Name.Size());

        VkDebugUtilsObjectNameInfoEXT VkNameInfo{};
        VkNameInfo.sType        = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        VkNameInfo.objectType   = static_cast<VkObjectType>(ObjectType);
        VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkSampler>(ObjectHandle));
        VkNameInfo.pObjectName  = NameStr.CStr();

        vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置Sampler DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}
