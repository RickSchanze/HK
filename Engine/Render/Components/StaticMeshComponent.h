#pragma once

#include "Render/Renderer/StaticMeshRenderer.h"
#include "RendererComponent.h"

class CStaticMeshComponent : public CRendererComponent
{
protected:
    // 静态网格渲染器
    FStaticMeshRenderer Renderer;

    // 要进行渲染的静态网格体
    HPROPERTY()
    TObjectPtr<HMesh> Mesh;

public:
    CStaticMeshComponent();

    void OnActive() override;
    void OnInactive() override;

    /**
     * 设置要进行渲染的静态网格体
     * @param InMesh
     */
    void SetMesh(HMesh* InMesh);
};
