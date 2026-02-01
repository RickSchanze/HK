//
// Created by Admin on 2026/2/1.
//

#include "RenderPipeline.h"

#include "RHI/RHIWindow.h"

FRenderPipeline::FRenderPipeline(bool bInRequireImGui) : bRequireImGui(bInRequireImGui)
{
    // TODO: 暂时获取主窗口
    auto MainWindow = FRHIWindowManager::GetRef().GetMainWindow();
    auto Size       = MainWindow->GetSize();
    for (int i = 0; i < HK_RENDER_INIT_FRAME_IN_FLIGHT; i++)
    {
        ColorBuffers[i] = MakeUnique<FRenderTexture>(Size.X, Size.Y);
        DepthBuffers[i] =
            MakeUnique<FRenderTexture>(1920, 1080, ERHIImageFormat::D32_SFloat, ERHIImageUsage::DepthStencilAttachment);
    }
}
