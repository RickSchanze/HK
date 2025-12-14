#include "TaskGraph/Task.h"

void FTask::Wait()
{
    if (RunningState.load(std::memory_order_acquire) == ETaskState::Completed ||
        RunningState.load(std::memory_order_acquire) == ETaskState::Failed)
    {
        return;
    }
    CompletionSemaphore.Wait();
}

FTask::FTask(FTask&& Other) noexcept
    : Task(std::move(Other.Task)), RunningState(Other.RunningState.load()),
      DependencyCount(Other.DependencyCount.load()), ExecutorLabel(Other.ExecutorLabel)
#ifdef HK_DEBUG
      ,
      DebugName(std::move(Other.DebugName))
#endif
{
}

FTask& FTask::operator=(FTask&& Other) noexcept
{
    if (this != &Other)
    {
        Task = std::move(Other.Task);
        RunningState.store(Other.RunningState.load());
        DependencyCount.store(Other.DependencyCount.load());
        ExecutorLabel = Other.ExecutorLabel;
#ifdef HK_DEBUG
        DebugName = std::move(Other.DebugName);
#endif
    }
    return *this;
}
