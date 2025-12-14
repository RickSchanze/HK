#include "TaskGraph/TaskGraph.h"
#include "Core/Logging/Logger.h"

#include <algorithm>

void FTaskGraph::StartUp()
{
    GameExecutor = std::make_unique<FGameExecutor>();
    RenderExecutor = std::make_unique<FRenderExecutor>();
    IOExecutor = std::make_unique<FIOExecutor>(4); // 4个IO线程

    HK_LOG_INFO(ELogcat::TaskGraph, "FTaskGraph initialized");
}

void FTaskGraph::ShutDown()
{
    if (IOExecutor)
    {
        IOExecutor->Shutdown();
    }
    if (RenderExecutor)
    {
        RenderExecutor->Shutdown();
    }
    if (GameExecutor)
    {
        GameExecutor->Shutdown();
    }

    HK_LOG_INFO(ELogcat::TaskGraph, "FTaskGraph shutdown");
}

void FTaskGraph::Launch(TSharedPtr<FTask> InTask)
{
    if (!InTask)
    {
        return;
    }

    // 检查依赖是否完成
    if (InTask->AreDependenciesComplete())
    {
        // 依赖已完成，直接提交到Executor
        EExecutorLabel ExecutorLabel = InTask->GetExecutorLabel();
        if (IExecutor* Executor = GetExecutor(ExecutorLabel))
        {
            // 设置状态为Pending，准备执行
            InTask->TrySetPending();
            TDelegate<void, TSharedPtr<FTask>> OnCompleteDelegate;
            OnCompleteDelegate.Bind([this](TSharedPtr<FTask> CompletedTask) {
                OnTaskComplete(CompletedTask);
            });
            Executor->SubmitTask(InTask, OnCompleteDelegate);
        }
    }
    else
    {
        // 依赖未完成，等待依赖完成后再提交
        // 状态设置为Pending
        InTask->TrySetPending();
    }
}

void FTaskGraph::Tick() const
{
    if (GameExecutor)
    {
        GameExecutor->Tick();
    }
}

IExecutor* FTaskGraph::GetExecutor(const EExecutorLabel Label) const
{
    switch (Label)
    {
        case EExecutorLabel::Game:
            return GameExecutor.get();
        case EExecutorLabel::Render:
            return RenderExecutor.get();
        case EExecutorLabel::IO:
            return IOExecutor.get();
        default:
            return nullptr;
    }
}

void FTaskGraph::RegisterDependencyCallback(TSharedPtr<FTask> Dependency,
                                            TSharedPtr<FTask> Dependent)
{
    // 检查依赖是否已经完成
    if (Dependency->IsCompleted())
    {
        // 依赖已完成，减少计数
        if (Dependent->DecrementDependency())
        {
            // 所有依赖都完成了，可以执行
            OnDependencyComplete(Dependent);
        }
    }
    else
    {
        // 依赖未完成，注册回调
        auto* DependentList = DependentTasks.Find(Dependency);
        if (!DependentList)
        {
            TThreadSafeArray<TSharedPtr<FTask>> NewList;
            NewList.PushBack(Dependent);
            DependentTasks.Insert(Dependency, std::move(NewList));
        }
        else
        {
            DependentList->PushBack(Dependent);
        }
    }
}

void FTaskGraph::OnDependencyComplete(TSharedPtr<FTask> Dependent)
{
    // 检查是否所有依赖都完成
    if (Dependent->AreDependenciesComplete())
    {
        // 提交到Executor执行
        EExecutorLabel ExecutorLabel = Dependent->GetExecutorLabel();
        if (IExecutor* Executor = GetExecutor(ExecutorLabel))
        {
            TDelegate<void, TSharedPtr<FTask>> OnCompleteDelegate;
            OnCompleteDelegate.Bind([this](TSharedPtr<FTask> CompletedTask) {
                OnTaskComplete(CompletedTask);
            });
            Executor->SubmitTask(Dependent, OnCompleteDelegate);
        }
    }
}

void FTaskGraph::OnTaskComplete(TSharedPtr<FTask> CompletedTask)
{
    // 查找所有依赖此任务的任务
    if (auto* DependentList = DependentTasks.Find(CompletedTask))
    {
        // 获取所有依赖任务
        auto Dependents = DependentList->Snapshot();
        for (auto& Dependent : Dependents)
        {
            // 减少依赖计数
            if (Dependent->DecrementDependency())
            {
                // 所有依赖都完成了
                OnDependencyComplete(Dependent);
            }
        }
        // 清理已完成的依赖关系
        DependentTasks.Remove(CompletedTask);
    }
}

