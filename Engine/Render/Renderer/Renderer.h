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
    ERendererType RendererType = ERendererType::Count;
    Int16 RendererMatrixIndex = -1;

    bool bVisible = true;

    void RegisterThisToModelMatrixPool();
    void UnregisterThisFromModelMatrixPool();

public:
    FRenderer();

    ~FRenderer();

    void SetVisible(bool InVisible);

    ERendererType GetRendererType() const
    {
        return RendererType;
    }
};