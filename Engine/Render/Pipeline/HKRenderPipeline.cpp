//
// Created by Admin on 2026/2/1.
//

#include "HKRenderPipeline.h"

#include "RHI/RHICommandBuffer.h"
#include "Render/RenderTarget.h"

void FHKRenderPipeline::Draw(FRHICommandBuffer& Commands, const FRenderPipelineDrawParams& Params)
{
    InitializeRenderTarget();
    Commands.BeginRendering(RenderTarget[Params.FrameIndex]);
    Commands.EndRendering();
}

void FHKRenderPipeline::InitializeRenderTarget()
{
    FRenderTargetColorAttachment Color;
    Color.ClearColor    = FVector4f(0.2f, 0.2f, 0.2f, 1.0f);
    Color.RenderTexture = ColorBuffers[0].Get();

    FRenderTargetDepthStencilAttachment Depth;
    Depth.RenderTexture = DepthBuffers[0].Get();

    RenderTarget[0] = FRenderTarget({Color}, Depth);

    Color.RenderTexture = ColorBuffers[1].Get();
    Depth.RenderTexture = DepthBuffers[1].Get();

    RenderTarget[1] = FRenderTarget({Color}, Depth);
}
