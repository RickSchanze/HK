#pragma once
#include "Core/Container/Array.h"
#include "Core/Singleton/Singleton.h"

class FRenderer;
/**
 * 渲染器管理器, 管理所有的渲染器
 */
class FRendererManager : public TSingleton<FRendererManager>
{
private:
    TArray<FRenderer*> Renderers;

public:
    void AddRenderer(FRenderer* InRenderer);
    void RemoveRenderer(FRenderer* InRenderer);

};
