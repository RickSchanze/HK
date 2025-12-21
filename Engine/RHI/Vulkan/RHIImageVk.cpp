//
// Created by Admin on 2025/12/21.
//

#include "GfxDeviceVk.h"

#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "RHI/RHIHandle.h"

#pragma region Image实现

FRHIImage FGfxDeviceVk::CreateImage(const FRHIImageDesc& ImageCreateInfo)
{
    try
    {
        // 创建 Vulkan 图像创建信息
        vk::ImageCreateInfo ImageInfo;
        ImageInfo.imageType = ConvertImageType(ImageCreateInfo.Type);
        ImageInfo.format = ConvertImageFormat(ImageCreateInfo.Format);
        ImageInfo.extent = vk::Extent3D(static_cast<uint32_t>(ImageCreateInfo.Extent.X),
                                        static_cast<uint32_t>(ImageCreateInfo.Extent.Y),
                                        static_cast<uint32_t>(ImageCreateInfo.Extent.Z));
        ImageInfo.mipLevels = ImageCreateInfo.MipLevels;
        ImageInfo.arrayLayers = ImageCreateInfo.ArrayLayers;
        ImageInfo.samples = ConvertSampleCount(ImageCreateInfo.Samples);
        ImageInfo.tiling = vk::ImageTiling::eOptimal; // 默认使用最优平铺
        ImageInfo.usage = ConvertImageUsage(ImageCreateInfo.Usage);
        ImageInfo.sharingMode = vk::SharingMode::eExclusive; // 默认独占模式
        ImageInfo.initialLayout = ConvertImageLayout(ImageCreateInfo.InitialLayout);

        // 创建 Vulkan 图像
        vk::Image VulkanImage;
        try
        {
            VulkanImage = Device.createImage(ImageInfo);
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_FATAL(ELogcat::RHI, "创建 Vulkan 图像失败: {}", e.what());
            throw std::runtime_error((FString("创建 Vulkan 图像失败: ") + FString(e.what())).CStr());
        }

        // 设置调试名称
        if (!ImageCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanImage, vk::ObjectType::eImage, ImageCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto& HandleManager = FRHIHandleManager::GetRef();
        FRHIHandle ImageHandle = HandleManager.CreateRHIHandle(
            ImageCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkImage>(VulkanImage)));

        // 创建 FRHIImage 对象
        FRHIImage Image;
        Image.Handle = ImageHandle;
        Image.Type = ImageCreateInfo.Type;
        Image.Format = ImageCreateInfo.Format;
        Image.Extent = ImageCreateInfo.Extent;
        Image.MipLevels = ImageCreateInfo.MipLevels;
        Image.ArrayLayers = ImageCreateInfo.ArrayLayers;
        Image.Samples = ImageCreateInfo.Samples;
        Image.Usage = ImageCreateInfo.Usage;

        HK_LOG_INFO(ELogcat::RHI, "图像创建成功: {}x{}x{} (格式: {}, MIP: {}, 层: {})", ImageCreateInfo.Extent.X,
                    ImageCreateInfo.Extent.Y, ImageCreateInfo.Extent.Z, static_cast<UInt32>(ImageCreateInfo.Format),
                    ImageCreateInfo.MipLevels, ImageCreateInfo.ArrayLayers);

        return Image;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建图像失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建图像失败: 未知异常");
        throw std::runtime_error("创建图像失败: 未知异常");
    }
}

void FGfxDeviceVk::DestroyImage(FRHIImage& Image)
{
    if (!Image.IsValid())
    {
        return;
    }

    // 获取 Vulkan 图像句柄
    const auto VulkanImage = vk::Image(Image.Handle.Cast<VkImage>());

    // 销毁 Vulkan 图像
    if (Device && VulkanImage)
    {
        Device.destroyImage(VulkanImage);
    }

    // 销毁 RHI 句柄
    auto& HandleManager = FRHIHandleManager::GetRef();
    HandleManager.DestroyRHIHandle(Image.Handle);

    // 重置图像对象
    Image = FRHIImage();

    HK_LOG_INFO(ELogcat::RHI, "图像已销毁");
}

#pragma endregion

#pragma region 转换函数实现

vk::Format FGfxDeviceVk::ConvertImageFormat(ERHIImageFormat Format)
{
    switch (Format)
    {
        case ERHIImageFormat::Undefined:
            return vk::Format::eUndefined;
        case ERHIImageFormat::R8G8B8A8_UNorm:
            return vk::Format::eR8G8B8A8Unorm;
        case ERHIImageFormat::R8G8B8A8_SNorm:
            return vk::Format::eR8G8B8A8Snorm;
        case ERHIImageFormat::R8G8B8A8_UScaled:
            return vk::Format::eR8G8B8A8Uscaled;
        case ERHIImageFormat::R8G8B8A8_SScaled:
            return vk::Format::eR8G8B8A8Sscaled;
        case ERHIImageFormat::R8G8B8A8_UInt:
            return vk::Format::eR8G8B8A8Uint;
        case ERHIImageFormat::R8G8B8A8_SInt:
            return vk::Format::eR8G8B8A8Sint;
        case ERHIImageFormat::R8G8B8A8_SRGB:
            return vk::Format::eR8G8B8A8Srgb;
        case ERHIImageFormat::B8G8R8A8_UNorm:
            return vk::Format::eB8G8R8A8Unorm;
        case ERHIImageFormat::B8G8R8A8_SRGB:
            return vk::Format::eB8G8R8A8Srgb;
        case ERHIImageFormat::R8G8B8_UNorm:
            return vk::Format::eR8G8B8Unorm;
        case ERHIImageFormat::R8G8B8_SNorm:
            return vk::Format::eR8G8B8Snorm;
        case ERHIImageFormat::R8G8B8_UScaled:
            return vk::Format::eR8G8B8Uscaled;
        case ERHIImageFormat::R8G8B8_SScaled:
            return vk::Format::eR8G8B8Sscaled;
        case ERHIImageFormat::R8G8B8_UInt:
            return vk::Format::eR8G8B8Uint;
        case ERHIImageFormat::R8G8B8_SInt:
            return vk::Format::eR8G8B8Sint;
        case ERHIImageFormat::R8G8B8_SRGB:
            return vk::Format::eR8G8B8Srgb;
        case ERHIImageFormat::R8_UNorm:
            return vk::Format::eR8Unorm;
        case ERHIImageFormat::R8_SNorm:
            return vk::Format::eR8Snorm;
        case ERHIImageFormat::R8_UScaled:
            return vk::Format::eR8Uscaled;
        case ERHIImageFormat::R8_SScaled:
            return vk::Format::eR8Sscaled;
        case ERHIImageFormat::R8_UInt:
            return vk::Format::eR8Uint;
        case ERHIImageFormat::R8_SInt:
            return vk::Format::eR8Sint;
        case ERHIImageFormat::R16_UNorm:
            return vk::Format::eR16Unorm;
        case ERHIImageFormat::R16_SNorm:
            return vk::Format::eR16Snorm;
        case ERHIImageFormat::R16_UScaled:
            return vk::Format::eR16Uscaled;
        case ERHIImageFormat::R16_SScaled:
            return vk::Format::eR16Sscaled;
        case ERHIImageFormat::R16_UInt:
            return vk::Format::eR16Uint;
        case ERHIImageFormat::R16_SInt:
            return vk::Format::eR16Sint;
        case ERHIImageFormat::R16_SFloat:
            return vk::Format::eR16Sfloat;
        case ERHIImageFormat::R16G16_UNorm:
            return vk::Format::eR16G16Unorm;
        case ERHIImageFormat::R16G16_SNorm:
            return vk::Format::eR16G16Snorm;
        case ERHIImageFormat::R16G16_UScaled:
            return vk::Format::eR16G16Uscaled;
        case ERHIImageFormat::R16G16_SScaled:
            return vk::Format::eR16G16Sscaled;
        case ERHIImageFormat::R16G16_UInt:
            return vk::Format::eR16G16Uint;
        case ERHIImageFormat::R16G16_SInt:
            return vk::Format::eR16G16Sint;
        case ERHIImageFormat::R16G16_SFloat:
            return vk::Format::eR16G16Sfloat;
        case ERHIImageFormat::R16G16B16_UNorm:
            return vk::Format::eR16G16B16Unorm;
        case ERHIImageFormat::R16G16B16_SNorm:
            return vk::Format::eR16G16B16Snorm;
        case ERHIImageFormat::R16G16B16_UScaled:
            return vk::Format::eR16G16B16Uscaled;
        case ERHIImageFormat::R16G16B16_SScaled:
            return vk::Format::eR16G16B16Sscaled;
        case ERHIImageFormat::R16G16B16_UInt:
            return vk::Format::eR16G16B16Uint;
        case ERHIImageFormat::R16G16B16_SInt:
            return vk::Format::eR16G16B16Sint;
        case ERHIImageFormat::R16G16B16_SFloat:
            return vk::Format::eR16G16B16Sfloat;
        case ERHIImageFormat::R16G16B16A16_UNorm:
            return vk::Format::eR16G16B16A16Unorm;
        case ERHIImageFormat::R16G16B16A16_SNorm:
            return vk::Format::eR16G16B16A16Snorm;
        case ERHIImageFormat::R16G16B16A16_UScaled:
            return vk::Format::eR16G16B16A16Uscaled;
        case ERHIImageFormat::R16G16B16A16_SScaled:
            return vk::Format::eR16G16B16A16Sscaled;
        case ERHIImageFormat::R16G16B16A16_UInt:
            return vk::Format::eR16G16B16A16Uint;
        case ERHIImageFormat::R16G16B16A16_SInt:
            return vk::Format::eR16G16B16A16Sint;
        case ERHIImageFormat::R16G16B16A16_SFloat:
            return vk::Format::eR16G16B16A16Sfloat;
        case ERHIImageFormat::R32_UInt:
            return vk::Format::eR32Uint;
        case ERHIImageFormat::R32_SInt:
            return vk::Format::eR32Sint;
        case ERHIImageFormat::R32_SFloat:
            return vk::Format::eR32Sfloat;
        case ERHIImageFormat::R32G32_UInt:
            return vk::Format::eR32G32Uint;
        case ERHIImageFormat::R32G32_SInt:
            return vk::Format::eR32G32Sint;
        case ERHIImageFormat::R32G32_SFloat:
            return vk::Format::eR32G32Sfloat;
        case ERHIImageFormat::R32G32B32_UInt:
            return vk::Format::eR32G32B32Uint;
        case ERHIImageFormat::R32G32B32_SInt:
            return vk::Format::eR32G32B32Sint;
        case ERHIImageFormat::R32G32B32_SFloat:
            return vk::Format::eR32G32B32Sfloat;
        case ERHIImageFormat::R32G32B32A32_UInt:
            return vk::Format::eR32G32B32A32Uint;
        case ERHIImageFormat::R32G32B32A32_SInt:
            return vk::Format::eR32G32B32A32Sint;
        case ERHIImageFormat::R32G32B32A32_SFloat:
            return vk::Format::eR32G32B32A32Sfloat;
        case ERHIImageFormat::D16_UNorm:
            return vk::Format::eD16Unorm;
        case ERHIImageFormat::D32_SFloat:
            return vk::Format::eD32Sfloat;
        case ERHIImageFormat::S8_UInt:
            return vk::Format::eS8Uint;
        case ERHIImageFormat::D16_UNorm_S8_UInt:
            return vk::Format::eD16UnormS8Uint;
        case ERHIImageFormat::D24_UNorm_S8_UInt:
            return vk::Format::eD24UnormS8Uint;
        case ERHIImageFormat::D32_SFloat_S8_UInt:
            return vk::Format::eD32SfloatS8Uint;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的图像格式: {}", static_cast<UInt32>(Format));
            return vk::Format::eUndefined;
    }
}

vk::ImageUsageFlags FGfxDeviceVk::ConvertImageUsage(ERHIImageUsage Usage)
{
    vk::ImageUsageFlags Flags = {};

    if (HasFlag(Usage, ERHIImageUsage::TransferSrc))
        Flags |= vk::ImageUsageFlagBits::eTransferSrc;
    if (HasFlag(Usage, ERHIImageUsage::TransferDst))
        Flags |= vk::ImageUsageFlagBits::eTransferDst;
    if (HasFlag(Usage, ERHIImageUsage::Sampled))
        Flags |= vk::ImageUsageFlagBits::eSampled;
    if (HasFlag(Usage, ERHIImageUsage::Storage))
        Flags |= vk::ImageUsageFlagBits::eStorage;
    if (HasFlag(Usage, ERHIImageUsage::ColorAttachment))
        Flags |= vk::ImageUsageFlagBits::eColorAttachment;
    if (HasFlag(Usage, ERHIImageUsage::DepthStencilAttachment))
        Flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    if (HasFlag(Usage, ERHIImageUsage::TransientAttachment))
        Flags |= vk::ImageUsageFlagBits::eTransientAttachment;
    if (HasFlag(Usage, ERHIImageUsage::InputAttachment))
        Flags |= vk::ImageUsageFlagBits::eInputAttachment;

    return Flags;
}

vk::ImageLayout FGfxDeviceVk::ConvertImageLayout(ERHIImageLayout Layout)
{
    switch (Layout)
    {
        case ERHIImageLayout::Undefined:
            return vk::ImageLayout::eUndefined;
        case ERHIImageLayout::General:
            return vk::ImageLayout::eGeneral;
        case ERHIImageLayout::ColorAttachmentOptimal:
            return vk::ImageLayout::eColorAttachmentOptimal;
        case ERHIImageLayout::DepthStencilAttachmentOptimal:
            return vk::ImageLayout::eDepthStencilAttachmentOptimal;
        case ERHIImageLayout::DepthStencilReadOnlyOptimal:
            return vk::ImageLayout::eDepthStencilReadOnlyOptimal;
        case ERHIImageLayout::ShaderReadOnlyOptimal:
            return vk::ImageLayout::eShaderReadOnlyOptimal;
        case ERHIImageLayout::TransferSrcOptimal:
            return vk::ImageLayout::eTransferSrcOptimal;
        case ERHIImageLayout::TransferDstOptimal:
            return vk::ImageLayout::eTransferDstOptimal;
        case ERHIImageLayout::Preinitialized:
            return vk::ImageLayout::ePreinitialized;
        case ERHIImageLayout::PresentSrcKHR:
            return vk::ImageLayout::ePresentSrcKHR;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的图像布局: {}", static_cast<UInt32>(Layout));
            return vk::ImageLayout::eUndefined;
    }
}

vk::ImageType FGfxDeviceVk::ConvertImageType(ERHIImageType Type)
{
    switch (Type)
    {
        case ERHIImageType::Image1D:
            return vk::ImageType::e1D;
        case ERHIImageType::Image2D:
            return vk::ImageType::e2D;
        case ERHIImageType::Image3D:
            return vk::ImageType::e3D;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的图像类型: {}", static_cast<UInt32>(Type));
            return vk::ImageType::e2D;
    }
}

vk::SampleCountFlagBits FGfxDeviceVk::ConvertSampleCount(ERHISampleCount Samples)
{
    switch (Samples)
    {
        case ERHISampleCount::Sample1:
            return vk::SampleCountFlagBits::e1;
        case ERHISampleCount::Sample2:
            return vk::SampleCountFlagBits::e2;
        case ERHISampleCount::Sample4:
            return vk::SampleCountFlagBits::e4;
        case ERHISampleCount::Sample8:
            return vk::SampleCountFlagBits::e8;
        case ERHISampleCount::Sample16:
            return vk::SampleCountFlagBits::e16;
        case ERHISampleCount::Sample32:
            return vk::SampleCountFlagBits::e32;
        case ERHISampleCount::Sample64:
            return vk::SampleCountFlagBits::e64;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的采样数量: {}", static_cast<UInt32>(Samples));
            return vk::SampleCountFlagBits::e1;
    }
}

#pragma endregion

void FGfxDeviceVk::SetDebugName(vk::Image ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const
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
            VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImage>(ObjectHandle));
            VkNameInfo.pObjectName = NameStr.CStr();

            vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
        }
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置Image DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}
