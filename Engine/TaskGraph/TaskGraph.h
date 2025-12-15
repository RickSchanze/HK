#pragma once

#include "Core/Container/Array.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/String.h"
#include "Core/Utility/SharedPtr.h"
#include "TaskGraph/Executor.h"
#include "TaskGraph/Task.h"
#include "TaskGraph/ThreadSafeArray.h"
#include "TaskGraph/ThreadSafeMap.h"

#include <memory>

class FTaskGraph : public FSingleton<FTaskGraph>
{
public:
    void StartUp() override;
    void ShutDown() override;

    // 默认在IO Executor上创建任务
    template <typename LambdaType, typename... Dependencies>
    TSharedPtr<FTask> Create(const FString& TaskDebugString, LambdaType&& TaskLambda, Dependencies... TaskDependencies)
    {
        return CreateInternal(TaskDebugString, EExecutorLabel::IO, std::forward<LambdaType>(TaskLambda),
                              TaskDependencies...);
    }

    // 在指定Executor上创建任务
    template <typename LambdaType, typename... Dependencies>
    TSharedPtr<FTask> Create(const FString& TaskDebugString, EExecutorLabel ExecutorLabel, LambdaType&& TaskLambda,
                             Dependencies... TaskDependencies)
    {
        return CreateInternal(TaskDebugString, ExecutorLabel, std::forward<LambdaType>(TaskLambda),
                              TaskDependencies...);
    }

    // Launch任务（传入已创建的任务）
    void Launch(TSharedPtr<FTask> InTask);

    // Launch任务（创建并执行，默认IO Executor）
    template <typename LambdaType, typename... Dependencies>
    void Launch(const FString& TaskDebugString, LambdaType&& TaskLambda, Dependencies... TaskDependencies)
    {
        auto Task = Create(TaskDebugString, std::forward<LambdaType>(TaskLambda), TaskDependencies...);
        Launch(Task);
    }

    // Launch任务（创建并执行，指定Executor）
    template <typename LambdaType, typename... Dependencies>
    void Launch(const FString& TaskDebugString, EExecutorLabel ExecutorLabel, LambdaType&& TaskLambda,
                Dependencies... TaskDependencies)
    {
        auto Task = Create(TaskDebugString, ExecutorLabel, std::forward<LambdaType>(TaskLambda), TaskDependencies...);
        Launch(Task);
    }

    // Game线程需要调用Tick来执行任务
    void Tick() const;

    // 获取Executor
    IExecutor* GetExecutor(EExecutorLabel Label) const;

private:
    template <typename LambdaType, typename... Dependencies>
    TSharedPtr<FTask> CreateInternal(const FString& TaskDebugString, EExecutorLabel ExecutorLabel,
                                     LambdaType&& TaskLambda, Dependencies... TaskDependencies)
    {
        auto Task = MakeShared<FTask>();
        FTask::TaskLambda Delegate;
        Delegate.Bind(std::forward<LambdaType>(TaskLambda));
        Task->SetTask(std::move(Delegate));
        Task->SetExecutorLabel(ExecutorLabel);

#ifdef HK_DEBUG
        Task->SetDebugName(TaskDebugString);
#endif

        // 收集依赖
        TArray<TSharedPtr<FTask>> DependenciesList;
        CollectDependencies(DependenciesList, TaskDependencies...);

        // 设置依赖计数
        Task->SetDependencyCount(DependenciesList.Size());

        // 为每个依赖注册完成回调
        for (auto& Dep : DependenciesList)
        {
            RegisterDependencyCallback(Dep, Task);
        }

        return Task;
    }

    template <typename First, typename... Rest>
    void CollectDependencies(TArray<TSharedPtr<FTask>>& OutDependencies, First FirstDep, Rest... RestDeps)
    {
        if constexpr (std::is_same_v<std::decay_t<First>, TSharedPtr<FTask>>)
        {
            OutDependencies.Add(FirstDep);
        }
        CollectDependencies(OutDependencies, RestDeps...);
    }

    template <typename First>
    void CollectDependencies(TArray<TSharedPtr<FTask>>& OutDependencies, First FirstDep)
    {
        if constexpr (std::is_same_v<std::decay_t<First>, TSharedPtr<FTask>>)
        {
            OutDependencies.Add(FirstDep);
        }
    }

    static void CollectDependencies(TArray<TSharedPtr<FTask>>& /*OutDependencies*/)
    {
        // 递归终止
    }

    void RegisterDependencyCallback(TSharedPtr<FTask> Dependency, TSharedPtr<FTask> Dependent);

    void OnDependencyComplete(TSharedPtr<FTask> Dependent);

    void OnTaskComplete(TSharedPtr<FTask> CompletedTask);

    std::unique_ptr<FGameExecutor> GameExecutor;
    std::unique_ptr<FRenderExecutor> RenderExecutor;
    std::unique_ptr<FIOExecutor> IOExecutor;

    // 存储等待依赖完成的任务
    TThreadSafeMap<TSharedPtr<FTask>, TThreadSafeArray<TSharedPtr<FTask>>> DependentTasks;
};
