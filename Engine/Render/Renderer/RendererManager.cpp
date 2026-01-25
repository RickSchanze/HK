//
// Created by Admin on 2026/1/25.
//

#include "RendererManager.h"

void FRendererManager::AddRenderer(FRenderer* InRenderer)
{
    if (InRenderer == nullptr)
    {
        return;
    }
    if (Renderers.Contains(InRenderer))
    {
        return;
    }
    Renderers.Add(InRenderer);
}

void FRendererManager::RemoveRenderer(FRenderer* InRenderer)
{
    Renderers.Remove(InRenderer);
}