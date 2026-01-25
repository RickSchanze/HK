//
// Created by Admin on 2026/1/25.
//

#include "StaticMeshComponent.h"

CStaticMeshComponent::CStaticMeshComponent()
{
    SetActive(true);
}

void CStaticMeshComponent::OnActive()
{
    Renderer.SetVisible(true);
}

void CStaticMeshComponent::OnInactive()
{
    Renderer.SetVisible(false);
}

void CStaticMeshComponent::SetMesh(HMesh* InMesh)
{
    Mesh = InMesh;
    Renderer.SetMesh(InMesh);
}