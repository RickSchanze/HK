#include "RenderContext.h"
#include "Core/Logging/Logger.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHICommandBuffer.h"
#include "RHI/RHICommandPool.h"
#include "RHI/RHIWindow.h"

void FRenderContext::StartUp()
{
    FGfxDevice& GfxDevice = GetGfxDeviceRef();

    // 创建全局上传命令池（使用 Graphics 队列族，通常也支持 Transfer 操作）
    FRHICommandPoolDesc PoolDesc;
    PoolDesc.Flags            = ERHICommandPoolCreateFlag::ResetCommandBuffer;
    PoolDesc.QueueFamilyIndex = 0; // 使用 Graphics 队列族（索引 0）
    PoolDesc.DebugName        = FString("GlobalUploadCommandPool");

    UploadCommandPool = GfxDevice.CreateCommandPool(PoolDesc);
    if (!UploadCommandPool.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Render, "Failed to create global upload command pool");
    }
    else
    {
        HK_LOG_INFO(ELogcat::Render, "Global upload command pool created");
    }

    // 创建帧同步资源
    FRHISemaphoreDesc SemaphoreDesc;
    FRHIFenceDesc     FenceDesc;
    FenceDesc.Flags = ERHIFenceCreateFlag::Signaled;

    for (UInt32 i = 0; i < HK_RENDER_INIT_FRAME_IN_FLIGHT; ++i)
    {
        // 创建信号量
        SemaphoreDesc.DebugName     = std::format("ImageAvailableSemaphore_{}", i);
        ImageAvailableSemaphores[i] = GfxDevice.CreateSemaphore(SemaphoreDesc);

        SemaphoreDesc.DebugName     = std::format("RenderFinishedSemaphore_{}", i);
        RenderFinishedSemaphores[i] = GfxDevice.CreateSemaphore(SemaphoreDesc);

        // 创建栅栏
        FenceDesc.DebugName = std::format("InFlightFence_{}", i);
        InFlightFences[i]   = GfxDevice.CreateFence(FenceDesc);

        // 创建命令池
        PoolDesc.DebugName   = std::format("FrameCommandPool_{}", i);
        FrameCommandPools[i] = GfxDevice.CreateCommandPool(PoolDesc);

        // 创建命令缓冲区
        FRHICommandBufferDesc CmdBufferDesc;
        CmdBufferDesc.Level     = ERHICommandBufferLevel::Primary;
        CmdBufferDesc.DebugName = std::format("FrameCommandBuffer_{}", i);
        FrameCommandBuffers[i]  = GfxDevice.CreateCommandBuffer(FrameCommandPools[i], CmdBufferDesc);

        if (!ImageAvailableSemaphores[i].IsValid() || !RenderFinishedSemaphores[i].IsValid() ||
            !InFlightFences[i].IsValid() || !FrameCommandPools[i].IsValid() || !FrameCommandBuffers[i].IsValid())
        {
            HK_LOG_ERROR(ELogcat::Render, "Failed to create frame synchronization resources for frame {}", i);
        }
    }

    HK_LOG_INFO(ELogcat::Render, "Frame synchronization resources created");
}

void FRenderContext::ShutDown()
{
    FGfxDevice& GfxDevice = GetGfxDeviceRef();

    // 等待设备空闲
    GfxDevice.WaitIdle();

    // 销毁帧同步资源
    for (UInt32 i = 0; i < HK_RENDER_INIT_FRAME_IN_FLIGHT; ++i)
    {
        if (ImageAvailableSemaphores[i].IsValid())
        {
            GfxDevice.DestroySemaphore(ImageAvailableSemaphores[i]);
        }
        if (RenderFinishedSemaphores[i].IsValid())
        {
            GfxDevice.DestroySemaphore(RenderFinishedSemaphores[i]);
        }
        if (InFlightFences[i].IsValid())
        {
            GfxDevice.DestroyFence(InFlightFences[i]);
        }
        if (FrameCommandBuffers[i].IsValid())
        {
            GfxDevice.DestroyCommandBuffer(FrameCommandPools[i], FrameCommandBuffers[i]);
        }
        if (FrameCommandPools[i].IsValid())
        {
            GfxDevice.DestroyCommandPool(FrameCommandPools[i]);
        }
    }

    if (UploadCommandPool.IsValid())
    {
        GfxDevice.DestroyCommandPool(UploadCommandPool);
        HK_LOG_INFO(ELogcat::Render, "Global upload command pool destroyed");
    }
}

void FRenderContext::RenderFrame()
{
    FGfxDevice& GfxDevice = GetGfxDeviceRef();

    // 获取主窗口
    FRHIWindow* MainWindow = FRHIWindowManager::GetRef().GetMainWindow();
    if (!MainWindow || !MainWindow->IsValid())
    {
        HK_LOG_ERROR(ELogcat::Render, "Main window is not valid");
        return;
    }

    // 获取当前帧的同步资源
    const UInt32       FrameIndex              = CurrentFrameIndex;
    FRHIFence&         InFlightFence           = InFlightFences[FrameIndex];
    FRHISemaphore&     ImageAvailableSemaphore = ImageAvailableSemaphores[FrameIndex];
    FRHISemaphore&     RenderFinishedSemaphore = RenderFinishedSemaphores[FrameIndex];
    FRHICommandBuffer& CmdBuffer               = FrameCommandBuffers[FrameIndex];

    // 1. 等待上一帧完成
    if (!GfxDevice.WaitForFence(InFlightFence, UINT64_MAX))
    {
        HK_LOG_ERROR(ELogcat::Render, "Failed to wait for in-flight fence");
        return;
    }

    // 2. 获取SwapChain的下一个图像
    UInt32 ImageIndex = 0;
    if (!GfxDevice.AcquireNextImage(*MainWindow, ImageAvailableSemaphore, ImageIndex))
    {
        HK_LOG_WARN(ELogcat::Render, "Failed to acquire next swapchain image, skipping frame");
        return;
    }

    // 3. 重置栅栏（在提交命令之前）
    if (!GfxDevice.ResetFence(InFlightFence))
    {
        HK_LOG_ERROR(ELogcat::Render, "Failed to reset fence");
        return;
    }

    // 4. 重置并开始记录命令缓冲区
    CmdBuffer.Reset(false);
    CmdBuffer.Begin(ERHICommandBufferUsageFlag::OneTimeSubmit);

    // ========================================================================
    // TODO: 这里留给用户实现实际的渲染命令
    // 用户可以在这里调用 BeginRendering、绘制命令、EndRendering 等
    // 例如:
    //   FRenderTarget RenderTarget = ...;
    //   Cmd.BeginRendering(RenderTarget);
    //   Cmd.SetViewport(...);
    //   Cmd.SetScissor(...);
    //   Cmd.BindPipeline(...);
    //   Cmd.Draw(...);
    //   Cmd.EndRendering();
    // ========================================================================

    // 5. 结束命令缓冲区记录
    CmdBuffer.End();

    // 6. 提交命令缓冲区
    // 等待 ImageAvailableSemaphore（图像可用后才能渲染）
    // 渲染完成后发出 RenderFinishedSemaphore 信号
    // 完成后发出 InFlightFence 信号
    if (!CmdBuffer.Submit({ImageAvailableSemaphore}, {RenderFinishedSemaphore}, InFlightFence))
    {
        HK_LOG_ERROR(ELogcat::Render, "Failed to submit command buffer");
        return;
    }

    // 7. 呈现图像
    // 等待 RenderFinishedSemaphore（渲染完成后才能呈现）
    if (!GfxDevice.PresentImage(*MainWindow, ImageIndex, RenderFinishedSemaphore))
    {
        HK_LOG_WARN(ELogcat::Render, "Failed to present swapchain image");
        return;
    }

    // 8. 更新帧索引
    CurrentFrameIndex = (CurrentFrameIndex + 1) % HK_RENDER_INIT_FRAME_IN_FLIGHT;
}
