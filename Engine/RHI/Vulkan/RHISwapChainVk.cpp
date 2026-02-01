//
// Created by Admin on 2026/2/1.
//

#include "GfxDeviceVk.h"
#include "Core/Logging/Logger.h"
#include "RHI/RHIWindow.h"

// ============================================================================
// SwapChain 图像获取和呈现
// ============================================================================

bool FGfxDeviceVk::AcquireNextImage(FRHIWindow& Window, const FRHISemaphore& ImageAvailableSemaphore,
                                    UInt32& OutImageIndex)
{
    if (!Window.IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid window");
        return false;
    }

    if (!ImageAvailableSemaphore.IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid semaphore");
        return false;
    }

    // 获取SwapChain
    auto SwapChainHandle = Window.GetSwapChain().Handle.Cast<VkSwapchainKHR>();
    auto VkSwapChain     = vk::SwapchainKHR(SwapChainHandle);
    if (!VkSwapChain)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid swapchain");
        return false;
    }

    // 获取信号量
    auto SemaphoreHandle = ImageAvailableSemaphore.GetHandle().Cast<VkSemaphore>();
    auto VkSemaphore     = vk::Semaphore(SemaphoreHandle);

    // 获取下一个图像
    try
    {
        vk::ResultValue<UInt32> Result = Device.acquireNextImageKHR(VkSwapChain, UINT64_MAX, VkSemaphore, nullptr);

        if (Result.result == vk::Result::eSuccess || Result.result == vk::Result::eSuboptimalKHR)
        {
            OutImageIndex = Result.value;
            return true;
        }
        else if (Result.result == vk::Result::eErrorOutOfDateKHR)
        {
            // SwapChain过期，需要重建
            HK_LOG_WARN(ELogcat::RHI, "SwapChain out of date, needs recreation");
            return false;
        }
        else
        {
            HK_LOG_ERROR(ELogcat::RHI, "Failed to acquire swapchain image: {}", static_cast<int>(Result.result));
            return false;
        }
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Vulkan error while acquiring swapchain image: {}", e.what());
        return false;
    }
}

bool FGfxDeviceVk::PresentImage(FRHIWindow& Window, UInt32 ImageIndex, const FRHISemaphore& RenderFinishedSemaphore)
{
    if (!Window.IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid window");
        return false;
    }

    if (!RenderFinishedSemaphore.IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid semaphore");
        return false;
    }

    // 获取SwapChain
    auto SwapChainHandle = Window.GetSwapChain().Handle.Cast<VkSwapchainKHR>();
    auto VkSwapChain     = vk::SwapchainKHR(SwapChainHandle);
    if (!VkSwapChain)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid swapchain");
        return false;
    }

    // 获取信号量
    auto SemaphoreHandle = RenderFinishedSemaphore.GetHandle().Cast<VkSemaphore>();
    auto VkSemaphore     = vk::Semaphore(SemaphoreHandle);

    // 构建呈现信息
    vk::PresentInfoKHR PresentInfo;
    PresentInfo.waitSemaphoreCount = 1;
    PresentInfo.pWaitSemaphores    = &VkSemaphore;
    PresentInfo.swapchainCount     = 1;
    PresentInfo.pSwapchains        = &VkSwapChain;
    PresentInfo.pImageIndices      = &ImageIndex;
    PresentInfo.pResults           = nullptr; // 可选

    // 呈现图像
    try
    {
        vk::Result Result = PresentQueue.presentKHR(PresentInfo);

        if (Result == vk::Result::eSuccess)
        {
            return true;
        }
        else if (Result == vk::Result::eSuboptimalKHR || Result == vk::Result::eErrorOutOfDateKHR)
        {
            // SwapChain次优或过期，但仍然成功呈现
            HK_LOG_WARN(ELogcat::RHI, "SwapChain suboptimal or out of date: {}", static_cast<int>(Result));
            return true;
        }
        else
        {
            HK_LOG_ERROR(ELogcat::RHI, "Failed to present swapchain image: {}", static_cast<int>(Result));
            return false;
        }
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Vulkan error while presenting swapchain image: {}", e.what());
        return false;
    }
}

FRHIImageView FGfxDeviceVk::GetSwapChainImageView(FRHIWindow& Window, UInt32 ImageIndex)
{
    if (!Window.IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Invalid window");
        return {};
    }

    // 查找窗口在数组中的索引
    FRHIWindowManager& WindowManager = FRHIWindowManager::GetRef();
    Int32              WindowIndex   = -1;
    for (Int32 i = 0; i < MAX_RHI_WINDOW_COUNT; ++i)
    {
        if (WindowManager.Windows[i] && WindowManager.Windows[i].Get() == &Window)
        {
            WindowIndex = i;
            break;
        }
    }

    if (WindowIndex < 0)
    {
        HK_LOG_ERROR(ELogcat::RHI, "Window not found in window manager");
        return {};
    }

    // 获取SwapChain数据
    FSwapChainData& SwapChainData = SwapChainDataArray[WindowIndex];
    if (ImageIndex >= SwapChainData.ImageViews.Size())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Image index {} out of range (max: {})", ImageIndex,
                     SwapChainData.ImageViews.Size());
        return {};
    }

    return SwapChainData.ImageViews[ImageIndex];
}
