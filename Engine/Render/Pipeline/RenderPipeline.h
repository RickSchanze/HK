#pragma once
#include "Core/Utility/UniquePtr.h"
#include "Render/Texture/RenderTexture.h"

#include "Render/RenderOptions.h"
#include "RenderPipeline.generated.h"

class FRHICommandBuffer;

struct FRenderPipelineDrawParams
{
    UInt32 FrameIndex;
};

HCLASS(Abstract)
class FRenderPipeline
{
    GENERATED_BODY(FRenderPipeline)
public:
    virtual ~FRenderPipeline() = default;
    FRenderPipeline(bool bInRequireImGui = false);

    virtual void Draw(FRHICommandBuffer& Commands, const FRenderPipelineDrawParams& Params) = 0;

protected:
    // 是否需要ImGui, 如果为true, 那么BackBuffer将是一张单独的RenderTexture, 而不是SwapChain
    bool bRequireImGui = false;

    TFixedArray<TUniquePtr<FRenderTexture>, HK_RENDER_INIT_FRAME_IN_FLIGHT> ColorBuffers;
    TFixedArray<TUniquePtr<FRenderTexture>, HK_RENDER_INIT_FRAME_IN_FLIGHT> DepthBuffers;
};
