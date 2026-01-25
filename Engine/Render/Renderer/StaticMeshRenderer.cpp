//
// Created by Admin on 2026/1/24.
//

#include "StaticMeshRenderer.h"

FStaticMeshRenderer::FStaticMeshRenderer()
{
    Mesh = nullptr;
}

FStaticMeshRenderer::~FStaticMeshRenderer()
{
    Mesh = nullptr;
}

void FStaticMeshRenderer::SetMesh(HMesh* InMesh)
{
    if (Mesh == InMesh)
    {
        return;
    }

    Mesh = InMesh;
    if (InMesh != nullptr)
    {
        bCanHasModelMatrix = true;
        if (bVisible)
        {
            RegisterThisToModelMatrixPool();
        }
    }
}