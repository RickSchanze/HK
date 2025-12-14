#include "TaskGraph/Executor.h"
#include "Core/Logging/Logger.h"

#include <sstream>

// FGameExecutor
FGameExecutor::FGameExecutor() : Name("Game") {}

FGameExecutor::~FGameExecutor() = default;

void FGameExecutor::SubmitTask(TSharedPtr<FTask> InTask, TDelegate<void, TSharedPtr<FTask>> OnComplete)
{
    if (!MyShutdown.load(std::memory_order_acquire))
    {
        TaskQueue.Push(TaskWithCallback{InTask, OnComplete});
    }
}

const FString& FGameExecutor::GetName() const
{
    return Name;
}

EExecutorLabel FGameExecutor::GetLabel() const
{
    return EExecutorLabel::Game;
}

void FGameExecutor::Shutdown()
{
    MyShutdown.store(true, std::memory_order_release);
}

void FGameExecutor::Tick()
{
    TaskWithCallback Item;
    while (TaskQueue.TryPop(Item))
    {
        Item.Task->Execute(
            [Item]()
            {
                if (Item.OnComplete.IsBound())
                {
                    Item.OnComplete.Invoke(Item.Task);
                }
            });
    }
}

// FRenderExecutor
FRenderExecutor::FRenderExecutor() : Name("Render")
{
    Thread = std::thread(&FRenderExecutor::Run, this);
}

FRenderExecutor::~FRenderExecutor()
{
    // Shutdown();
    if (Thread.joinable())
    {
        Thread.join();
    }
}

void FRenderExecutor::SubmitTask(TSharedPtr<FTask> InTask, TDelegate<void, TSharedPtr<FTask>> OnComplete)
{
    if (!MyShutdown.load(std::memory_order_acquire))
    {
        TaskQueue.Push(TaskWithCallback{InTask, OnComplete});
    }
}

const FString& FRenderExecutor::GetName() const
{
    return Name;
}

EExecutorLabel FRenderExecutor::GetLabel() const
{
    return EExecutorLabel::Render;
}

void FRenderExecutor::Shutdown()
{
    MyShutdown.store(true, std::memory_order_release);
}

void FRenderExecutor::Run()
{
    while (!MyShutdown.load(std::memory_order_acquire))
    {
        TaskWithCallback Item;
        if (TaskQueue.TryPop(Item))
        {
            Item.Task->Execute(
                [Item]()
                {
                    if (Item.OnComplete.IsBound())
                    {
                        Item.OnComplete.Invoke(Item.Task);
                    }
                });
        }
        else
        {
            // 没有任务时短暂休眠，避免CPU空转
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

// FIOExecutor
FIOExecutor::FIOExecutor(size_t ThreadCount) : Name("IO")
{
    Threads.Reserve(ThreadCount);
    for (size_t i = 0; i < ThreadCount; ++i)
    {
        Threads.Emplace(&FIOExecutor::WorkerThread, this, i);
    }
}

FIOExecutor::~FIOExecutor()
{
    for (auto& Thread : Threads)
    {
        if (Thread.joinable())
        {
            Thread.join();
        }
    }
}

void FIOExecutor::SubmitTask(TSharedPtr<FTask> InTask, TDelegate<void, TSharedPtr<FTask>> OnComplete)
{
    if (!MyShutdown.load(std::memory_order_acquire))
    {
        TaskQueue.Push(TaskWithCallback{InTask, OnComplete});
    }
}

const FString& FIOExecutor::GetName() const
{
    return Name;
}

EExecutorLabel FIOExecutor::GetLabel() const
{
    return EExecutorLabel::IO;
}

void FIOExecutor::Shutdown()
{
    MyShutdown.store(true, std::memory_order_release);
}

void FIOExecutor::WorkerThread(size_t /*ThreadIndex*/)
{
    while (!MyShutdown.load(std::memory_order_acquire))
    {
        TaskWithCallback Item;
        if (TaskQueue.TryPop(Item))
        {
            Item.Task->Execute(
                [Item]()
                {
                    if (Item.OnComplete.IsBound())
                    {
                        Item.OnComplete.Invoke(Item.Task);
                    }
                });
        }
        else
        {
            // 没有任务时短暂休眠
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
