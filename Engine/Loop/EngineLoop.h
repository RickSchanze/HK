#pragma once
#include "Core/Time/TimePoint.h"
#include "Core/Utility/Macros.h"

class HK_API FEngineLoop
{
public:
    using FRenderTickFunc = void (*)(void);
    using FInputTickFunc = void (*)(void);

    void Init() const;
    void Run();
    void UnInit();

    void PreTick();
    void Tick();
    void PostTick();

    // 设置渲染和输入Tick函数
    void SetRenderTickFunc(FRenderTickFunc InFunc) { RenderTickFunc = InFunc; }
    void SetInputTickFunc(FInputTickFunc InFunc) { InputTickFunc = InFunc; }

private:
    FRenderTickFunc RenderTickFunc = nullptr;
    FInputTickFunc InputTickFunc = nullptr;
    bool bIsRunning = false;
};
