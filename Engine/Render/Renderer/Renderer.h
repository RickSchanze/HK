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

public:
    FRenderer();

    ~FRenderer();

    ERendererType GetRendererType() const
    {
        return RendererType;
    }
};