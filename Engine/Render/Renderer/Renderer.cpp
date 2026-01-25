//
// Created by Admin on 2026/1/24.
//

#include "Render/Renderer/Renderer.h"

#include "Render/GlobalRenderResources.h"

FRenderer::~FRenderer()
{
    SetVisible(false);
}

void FRenderer::SetVisible(bool InVisible)
{
    if (bVisible == InVisible)
    {
        return;
    }
    bVisible = InVisible;
    if (bVisible)
    {
        RegisterThisToModelMatrixPool();
    }
    else
    {
        UnregisterThisFromModelMatrixPool();
    }
}

void FRenderer::RegisterThisToModelMatrixPool()
{
    RendererMatrixIndex = FGlobalDynamicRenderResourcePool::GetRef().AddRendererIndexMap(this);
}

void FRenderer::UnregisterThisFromModelMatrixPool()
{
    RendererMatrixIndex = -1;
    FGlobalDynamicRenderResourcePool::GetRef().RemoveRendererIndexMap(this);
}

FRenderer::FRenderer()
{
    SetVisible(true);
}
