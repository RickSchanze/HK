//
// Created by Admin on 2026/1/25.
//

#include "RendererComponent.h"

#include "Render/GlobalRenderResources.h"

void CRendererComponent::OnTransformUpdated()
{
    if (RendererModelMatrixIndex == -1)
    {
        return;
    }
    const FMatrix4x4f WorldMatrix = WorldTransform.ToMatrix();
    FGlobalDynamicRenderResourcePool::GetRef().UpdateModelMatrix(WorldMatrix, RendererModelMatrixIndex);
}