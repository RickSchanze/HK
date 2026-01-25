#pragma once
#include "Renderer.h"

class HMesh;

class FStaticMeshRenderer : public FRenderer
{
private:
    HMesh* Mesh = nullptr;

public:
    FStaticMeshRenderer();
    ~FStaticMeshRenderer() override;

    void SetMesh(HMesh* InMesh);
};
