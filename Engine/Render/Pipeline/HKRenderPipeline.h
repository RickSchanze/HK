#pragma once

#include "HKRenderPipeline.generated.h"
#include "RenderPipeline.h"

#include "Render/RenderTarget.h"

HCLASS()
class FHKRenderPipeline : public FRenderPipeline
{
    GENERATED_BODY(FHKRenderPipeline)

public:
    void Draw(FRHICommandBuffer& Commands, const FRenderPipelineDrawParams& Params) override;

protected:
    void InitializeRenderTarget();

private:
    bool bRenderTargetInitialized = false;

    TFixedArray<FRenderTarget, HK_RENDER_INIT_FRAME_IN_FLIGHT> RenderTarget;
};