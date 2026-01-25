#pragma once

#include "Object/SceneComponent.h"

HCLASS()
class CRendererComponent : public CSceneComponent
{
protected:
    // 渲染器使用的模型矩阵索引
    Int16 RendererModelMatrixIndex = -1;

    void OnTransformUpdated() override;
};