#pragma once
#include "Core/Utility/Macros.h"

enum class ERendererType
{
    StaticMesh,
    Count,
};

class FRenderer
{
protected:
    // Renderer的类型
    ERendererType RendererType = ERendererType::Count;
    // Renderer在ModelMatrixPool中的索引
    Int16 RendererMatrixIndex = -1;
    // 是否可见
    bool bVisible = true;
    // 是否可以有模型矩阵
    bool bCanHasModelMatrix = false;

    void RegisterThisToModelMatrixPool();
    void UnregisterThisFromModelMatrixPool();

public:
    FRenderer();

    virtual ~FRenderer();

    void SetVisible(bool InVisible);

    ERendererType GetRendererType() const
    {
        return RendererType;
    }
};