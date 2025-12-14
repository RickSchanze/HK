#pragma once

#include "Core/Event/Delegate.h"
#include "TaskGraph/Semaphore.h"

#include <atomic>
#include <memory>
#include <type_traits>

#ifdef HK_DEBUG
#include "Core/String/String.h"
#endif

// 前向声明 - 定义在Executor.h中
enum class EExecutorLabel : int;

enum class ETaskState
{
    Created,   // 已创建但未启动
    Pending,   // 等待依赖完成
    Running,   // 正在执行
    Completed, // 已完成
    Failed     // 执行失败
};

class FTask
{
public:
    using TaskLambda = TDelegate<void>;

    FTask() = default;
    ~FTask() = default;

    // 禁止拷贝
    FTask(const FTask&) = delete;
    FTask& operator=(const FTask&) = delete;

    // 允许移动
    FTask(FTask&& Other) noexcept;
    FTask& operator=(FTask&& Other) noexcept;

    // 等待任务完成
    void Wait();

    // 查询任务状态
    ETaskState GetState() const
    {
        return RunningState.load(std::memory_order_acquire);
    }

    // 检查是否完成
    bool IsCompleted() const
    {
        ETaskState State = RunningState.load(std::memory_order_acquire);
        return State == ETaskState::Completed || State == ETaskState::Failed;
    }

    // 检查是否正在运行
    bool IsRunning() const
    {
        return RunningState.load(std::memory_order_acquire) == ETaskState::Running;
    }

    // 内部方法：设置任务函数
    void SetTask(TaskLambda InTask)
    {
        Task = std::move(InTask);
    }

    // 内部方法：设置ExecutorLabel
    void SetExecutorLabel(EExecutorLabel InLabel)
    {
        ExecutorLabel = InLabel;
    }

    // 内部方法：获取ExecutorLabel
    EExecutorLabel GetExecutorLabel() const
    {
        return ExecutorLabel;
    }

    // 内部方法：执行任务，完成后调用OnComplete回调
    template <typename OnCompleteCallback>
    void Execute(OnCompleteCallback OnComplete)
    {
        ETaskState Expected = ETaskState::Pending;
        if (!RunningState.compare_exchange_strong(Expected, ETaskState::Running, std::memory_order_acq_rel))
        {
            return; // 状态不是Pending，可能已经被执行或取消
        }

        try
        {
            if (Task.IsBound())
            {
                Task.Invoke();
            }
            RunningState.store(ETaskState::Completed, std::memory_order_release);
        }
        catch (...)
        {
            RunningState.store(ETaskState::Failed, std::memory_order_release);
        }

        // 通知等待的线程
        CompletionSemaphore.Signal();

        // 调用完成回调（lambda总是可调用的，直接调用）
        if constexpr (std::is_invocable_v<OnCompleteCallback>)
        {
            OnComplete();
        }
    }

    // 内部方法：设置依赖计数
    void SetDependencyCount(size_t Count)
    {
        DependencyCount.store(Count, std::memory_order_release);
    }

    // 内部方法：减少依赖计数，返回是否所有依赖都完成
    bool DecrementDependency()
    {
        size_t Previous = DependencyCount.fetch_sub(1, std::memory_order_acq_rel);
        return Previous == 1; // 如果之前是1，现在就是0，所有依赖都完成了
    }

    // 内部方法：检查依赖是否完成
    bool AreDependenciesComplete() const
    {
        return DependencyCount.load(std::memory_order_acquire) == 0;
    }

    // 内部方法：设置状态为Pending（用于Launch）
    bool TrySetPending()
    {
        ETaskState Expected = ETaskState::Created;
        return RunningState.compare_exchange_strong(Expected, ETaskState::Pending, std::memory_order_acq_rel);
    }

#ifdef HK_DEBUG
    void SetDebugName(const FString& InName)
    {
        DebugName = InName;
    }

    const FString& GetDebugName() const
    {
        return DebugName;
    }
#endif

private:
    TaskLambda Task;
    std::atomic<ETaskState> RunningState{ETaskState::Created};
    std::atomic<size_t> DependencyCount{0};
    FSemaphore CompletionSemaphore{0};
    EExecutorLabel ExecutorLabel{static_cast<EExecutorLabel>(2)}; // EExecutorLabel::IO = 2

#ifdef HK_DEBUG
    FString DebugName;
#endif
};
