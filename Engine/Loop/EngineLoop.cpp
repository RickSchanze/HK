//
// Created by Admin on 2025/12/18.
//

#include "EngineLoop.h"
#include "Core/Logging/Logger.h"
#include "Core/Utility/Profiler.h"
#include "EngineLoopEvents.h"
#include "LoopData.h"
#include "RHI/GfxDevice.h"

void FEngineLoop::Init() const
{
    HK_PROFILE_SCOPE_N("FEngineLoop::Init");

    // 检查必要的函数是否已设置
    if (RenderTickFunc == nullptr)
    {
        HK_LOG_WARN(ELogcat::Engine, "警告: RenderTickFunc未设置，渲染功能可能无法正常工作");
    }

    if (InputTickFunc == nullptr)
    {
        HK_LOG_WARN(ELogcat::Engine, "警告: InputTickFunc未设置，输入处理功能可能无法正常工作");
    }

    // 初始化循环数据
    GLoopData.FrameNumber = 0;
    GLoopData.ShouldCloseEngine = false;
    GLoopData.LastFrameTime = FTimePoint::Now();
    GLoopData.DeltaTime = FTimeDuration();

    // 初始化图形
    CreateGfxDevice();

    HK_LOG_INFO(ELogcat::Engine, "引擎循环初始化完成");
}

void FEngineLoop::UnInit()
{
    HK_PROFILE_SCOPE_N("FEngineLoop::UnInit");

    bIsRunning = false;
    HK_LOG_INFO(ELogcat::Engine, "引擎循环清理完成");
}

void FEngineLoop::Run()
{
    HK_PROFILE_SCOPE_N("FEngineLoop::Run");

    DestroyGfxDevice();

    bIsRunning = true;
    HK_LOG_INFO(ELogcat::Engine, "引擎循环开始运行");

    while (!GLoopData.ShouldCloseEngine && bIsRunning)
    {
        HK_PROFILE_FRAME_MARK();

        // 计算DeltaTime
        FTimePoint CurrentTime = FTimePoint::Now();
        GLoopData.DeltaTime = CurrentTime - GLoopData.LastFrameTime;
        GLoopData.LastFrameTime = CurrentTime;
        GLoopData.FrameNumber++;

        // 绘制DeltaTime到性能分析器
        HK_PROFILE_PLOT_F("DeltaTime (ms)", GLoopData.DeltaTime.AsFloat<FMilliseconds>());

        // PreTick阶段
        PreTick();

        // Tick阶段
        Tick();

        // PostTick阶段
        PostTick();
    }

    HK_LOG_INFO(ELogcat::Engine, "引擎循环结束运行");
}

void FEngineLoop::PreTick()
{
    HK_PROFILE_SCOPE_N("FEngineLoop::PreTick");

    // 触发PreTick事件
    GEngineLoopEvents.OnPreTick.Invoke();
}

void FEngineLoop::Tick()
{
    HK_PROFILE_SCOPE_N("FEngineLoop::Tick");

    // 触发Tick事件
    GEngineLoopEvents.OnTick.Invoke();

    // 调用输入Tick函数
    if (InputTickFunc != nullptr)
    {
        HK_PROFILE_SCOPE_N("InputTick");
        InputTickFunc();
    }

    // 调用渲染Tick函数
    if (RenderTickFunc != nullptr)
    {
        HK_PROFILE_SCOPE_N("RenderTick");
        RenderTickFunc();
    }
}

void FEngineLoop::PostTick()
{
    HK_PROFILE_SCOPE_N("FEngineLoop::PostTick");

    // 触发PostTick事件
    GEngineLoopEvents.OnPostTick.Invoke();
}