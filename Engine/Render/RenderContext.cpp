//
// Created by hulkzhou on 2025/12/22.
//

#include "RenderContext.h"
#include "Core/Logging/Logger.h"
#include "RHI/GfxDevice.h"
#include "RHI/RHICommandPool.h"

void FRenderContext::StartUp()
{
    FGfxDevice& GfxDevice = GetGfxDeviceRef();

    // 创建全局上传命令池（使用 Graphics 队列族，通常也支持 Transfer 操作）
    FRHICommandPoolDesc PoolDesc;
    PoolDesc.Flags            = ERHICommandPoolCreateFlag::ResetCommandBuffer;
    PoolDesc.QueueFamilyIndex = 0; // 使用 Graphics 队列族（索引 0）
    PoolDesc.DebugName        = "GlobalUploadCommandPool";

    UploadCommandPool = GfxDevice.CreateCommandPool(PoolDesc);
    if (!UploadCommandPool.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Render, "Failed to create global upload command pool");
    }
    else
    {
        HK_LOG_INFO(ELogcat::Render, "Global upload command pool created");
    }
}

void FRenderContext::ShutDown()
{
    if (UploadCommandPool.IsValid())
    {
        FGfxDevice& GfxDevice = GetGfxDeviceRef();
        GfxDevice.DestroyCommandPool(UploadCommandPool);
        HK_LOG_INFO(ELogcat::Render, "Global upload command pool destroyed");
    }
}
