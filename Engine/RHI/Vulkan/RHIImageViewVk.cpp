//
// Created by Admin on 2025/12/21.
//

#include "GfxDeviceVk.h"

#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "RHI/RHIHandle.h"

#pragma region ImageView实现

FRHIImageView FGfxDeviceVk::CreateImageView(const FRHIImage& Image, const FRHIImageViewDesc& ViewCreateInfo)
{
    HK_ASSERT_MSG_RAW(Image.IsValid(), "无效的图像句柄");

    try
    {
        // 创建 Vulkan 图像视图创建信息
        vk::ImageViewCreateInfo ViewInfo;
        ViewInfo.image = vk::Image(Image.Handle.Cast<VkImage>());
        ViewInfo.viewType = static_cast<vk::ImageViewType>(ConvertImageType(ViewCreateInfo.ViewType));
        ViewInfo.format = ConvertImageFormat(ViewCreateInfo.Format);

        // 设置组件映射（默认RGBA）
        ViewInfo.components = vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
                                                   vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA);

        // 设置子资源范围
        vk::ImageSubresourceRange SubresourceRange;

        // 转换图像方面到 Vulkan 方面掩码
        if (HasFlag(ViewCreateInfo.Aspects, ERHIImageAspect::Color))
        {
            SubresourceRange.aspectMask |= vk::ImageAspectFlagBits::eColor;
        }
        if (HasFlag(ViewCreateInfo.Aspects, ERHIImageAspect::Depth))
        {
            SubresourceRange.aspectMask |= vk::ImageAspectFlagBits::eDepth;
        }
        if (HasFlag(ViewCreateInfo.Aspects, ERHIImageAspect::Stencil))
        {
            SubresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }

        SubresourceRange.baseMipLevel = ViewCreateInfo.BaseMipLevel;
        SubresourceRange.levelCount = ViewCreateInfo.LevelCount;
        SubresourceRange.baseArrayLayer = ViewCreateInfo.BaseArrayLayer;
        SubresourceRange.layerCount = ViewCreateInfo.LayerCount;

        ViewInfo.subresourceRange = SubresourceRange;

        // 创建 Vulkan 图像视图
        vk::ImageView VulkanImageView;
        try
        {
            VulkanImageView = Device.createImageView(ViewInfo);
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_FATAL(ELogcat::RHI, "创建 Vulkan 图像视图失败: {}", e.what());
            throw std::runtime_error((FString("创建 Vulkan 图像视图失败: ") + FString(e.what())).CStr());
        }

        // 设置调试名称
        if (!ViewCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanImageView, vk::ObjectType::eImageView, ViewCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto& HandleManager = FRHIHandleManager::GetRef();
        FRHIHandle ViewHandle = HandleManager.CreateRHIHandle(
            ViewCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkImageView>(VulkanImageView)));

        // 创建 FRHIImageView 对象
        FRHIImageView ImageView;
        ImageView.Handle = ViewHandle;
        ImageView.Image = &Image; // 弱引用
        ImageView.ViewType = ViewCreateInfo.ViewType;
        ImageView.Format = ViewCreateInfo.Format;
        ImageView.Aspects = ViewCreateInfo.Aspects;

        HK_LOG_INFO(ELogcat::RHI, "图像视图创建成功: MIP[{}, {}], 层[{}, {}]", ViewCreateInfo.BaseMipLevel,
                    ViewCreateInfo.LevelCount, ViewCreateInfo.BaseArrayLayer, ViewCreateInfo.LayerCount);

        return ImageView;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建图像视图失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建图像视图失败: 未知异常");
        throw std::runtime_error("创建图像视图失败: 未知异常");
    }
}

void FGfxDeviceVk::DestroyImageView(FRHIImageView& ImageView)
{
    if (!ImageView.IsValid())
    {
        return;
    }

    // 获取 Vulkan 图像视图句柄
    const auto VulkanImageView = vk::ImageView(ImageView.Handle.Cast<VkImageView>());

    // 销毁 Vulkan 图像视图
    if (Device && VulkanImageView)
    {
        Device.destroyImageView(VulkanImageView);
    }

    // 销毁 RHI 句柄
    auto& HandleManager = FRHIHandleManager::GetRef();
    HandleManager.DestroyRHIHandle(ImageView.Handle);

    // 重置图像视图对象
    ImageView = FRHIImageView();

    HK_LOG_INFO(ELogcat::RHI, "图像视图已销毁");
}

#pragma endregion

void FGfxDeviceVk::SetDebugName(vk::ImageView ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const
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
            VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkImageView>(ObjectHandle));
            VkNameInfo.pObjectName = NameStr.CStr();

            vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
        }
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置ImageView DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}
