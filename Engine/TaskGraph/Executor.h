#pragma once

#include "Core/Container/Array.h"
#include "Core/Event/Delegate.h"
#include "Core/String/String.h"
#include "Core/Utility/SharedPtr.h"
#include "TaskGraph/Task.h"
#include "TaskGraph/ThreadSafeQueue.h"

#include <atomic>
#include <memory>
#include <thread>

enum class EExecutorLabel
{
    Game,
    Render,
    IO
};

// 任务和回调的结构
struct TaskWithCallback
{
    TSharedPtr<FTask> Task;
    TDelegate<void, TSharedPtr<FTask>> OnComplete;
};

class IExecutor
{
public:
    virtual ~IExecutor() = default;
    virtual void SubmitTask(TSharedPtr<FTask> InTask, TDelegate<void, TSharedPtr<FTask>> OnComplete) = 0;
    virtual const FString& GetName() const = 0;
    virtual EExecutorLabel GetLabel() const = 0;
    virtual void Shutdown() = 0;
};

// Game线程Executor - 主线程，需要主动Tick
class FGameExecutor : public IExecutor
{
public:
    FGameExecutor();
    ~FGameExecutor() override;

    void SubmitTask(TSharedPtr<FTask> InTask, TDelegate<void, TSharedPtr<FTask>> OnComplete) override;
    const FString& GetName() const override;
    EExecutorLabel GetLabel() const override;
    void Shutdown() override;

    // Game线程需要主动调用Tick来执行任务
    void Tick();

private:
    FString Name;
    TThreadSafeQueue<TaskWithCallback> TaskQueue;
    std::atomic<bool> MyShutdown{false};
};

// Render线程Executor - 常驻线程，顺序执行任务
class FRenderExecutor : public IExecutor
{
public:
    FRenderExecutor();
    ~FRenderExecutor() override;

    void SubmitTask(TSharedPtr<FTask> InTask, TDelegate<void, TSharedPtr<FTask>> OnComplete) override;
    const FString& GetName() const override;
    EExecutorLabel GetLabel() const override;
    void Shutdown() override;

private:
    void Run();

    FString Name;
    TThreadSafeQueue<TaskWithCallback> TaskQueue;
    std::atomic<bool> MyShutdown{false};
    std::thread Thread;
};

// IO线程池Executor - 线程池，多个线程并发执行
class FIOExecutor : public IExecutor
{
public:
    explicit FIOExecutor(size_t ThreadCount = 4);
    ~FIOExecutor() override;

    void SubmitTask(TSharedPtr<FTask> InTask, TDelegate<void, TSharedPtr<FTask>> OnComplete) override;
    const FString& GetName() const override;
    EExecutorLabel GetLabel() const override;
    void Shutdown() override;

private:
    void WorkerThread(size_t ThreadIndex);

    FString Name;
    TThreadSafeQueue<TaskWithCallback> TaskQueue;
    std::atomic<bool> MyShutdown{false};
    TArray<std::thread> Threads;
};
