#ifdef HK_WINDOWS
#include <windows.h>
#endif
#include "Core/Container/Array.h"
#include "Core/Logging/Logger.h"
#include "Core/Reflection/Property.h"
#include "Core/Reflection/TypeManager.h"
#include "Core/Serialization/BinaryArchive.h"
#include "Core/Serialization/JsonArchive.h"
#include "Core/Serialization/XMLArchive.h"
#include "Core/String/String.h"
#include "Math/Color.h"
#include "TaskGraph/TaskGraph.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

void TestBasicTask()
{
    HK_LOG_INFO(ELogcat::Test, "=== 测试1: 基本任务执行 ===");
    auto& TaskGraph = FTaskGraph::GetRef();

    std::atomic<int> Counter{0};

    // 创建一个简单任务
    auto Task = TaskGraph.Create(FString("BasicTask"),
                                 [&Counter]()
                                 {
                                     std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                     Counter = 42;
                                     HK_LOG_INFO(ELogcat::Test, "任务执行完成，Counter = {}", Counter.load());
                                 });

    HK_LOG_INFO(ELogcat::Test, "任务已创建，状态: {}", static_cast<int>(Task->GetState()));
    HK_LOG_INFO(ELogcat::Test, "启动任务...");
    TaskGraph.Launch(Task);

    // Game线程Tick执行任务
    while (!Task->IsCompleted())
    {
        TaskGraph.Tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    Task->Wait();
    HK_LOG_INFO(ELogcat::Test, "任务完成，Counter = {}", Counter.load());
    HK_ASSERT_RAW(Counter.load() == 42);

    TaskGraph.ShutDown();
    HK_LOG_INFO(ELogcat::Test, "测试1通过\n");
}

void TestTaskDependencies()
{
    HK_LOG_INFO(ELogcat::Test, "=== 测试2: 任务依赖 ===");
    auto& TaskGraph = FTaskGraph::GetRef();
    TaskGraph.StartUp();

    TArray<int> ExecutionOrder;
    std::mutex OrderMutex;

    // 创建任务A
    auto TaskA = TaskGraph.Create(FString("TaskA"), EExecutorLabel::IO,
                                  [&ExecutionOrder, &OrderMutex]()
                                  {
                                      std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                      std::lock_guard<std::mutex> Lock(OrderMutex);
                                      ExecutionOrder.Add(1);
                                      HK_LOG_INFO(ELogcat::Test, "TaskA执行完成");
                                  });

    // 创建任务B，依赖TaskA
    auto TaskB = TaskGraph.Create(
        FString("TaskB"), EExecutorLabel::IO,
        [&ExecutionOrder, &OrderMutex]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::lock_guard<std::mutex> Lock(OrderMutex);
            ExecutionOrder.Add(2);
            HK_LOG_INFO(ELogcat::Test, "TaskB执行完成");
        },
        TaskA);

    // 创建任务C，依赖TaskA和TaskB
    auto TaskC = TaskGraph.Create(
        FString("TaskC"), EExecutorLabel::IO,
        [&ExecutionOrder, &OrderMutex]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::lock_guard<std::mutex> Lock(OrderMutex);
            ExecutionOrder.Add(3);
            HK_LOG_INFO(ELogcat::Test, "TaskC执行完成");
        },
        TaskA, TaskB);

    HK_LOG_INFO(ELogcat::Test, "启动所有任务...");
    TaskGraph.Launch(TaskA);
    TaskGraph.Launch(TaskB);
    TaskGraph.Launch(TaskC);

    // 等待所有任务完成
    TaskC->Wait();
    TaskB->Wait();
    TaskA->Wait();

    HK_LOG_INFO(ELogcat::Test, "执行顺序: ");
    for (int Order : ExecutionOrder)
    {
        HK_LOG_INFO(ELogcat::Test, "  {}", Order);
    }

    // 验证执行顺序：A应该在B之前，B应该在C之前
    HK_ASSERT_RAW(ExecutionOrder.Size() == 3);
    HK_ASSERT_RAW(ExecutionOrder[0] == 1); // A先执行
    HK_ASSERT_RAW(ExecutionOrder[1] == 2); // B在A之后
    HK_ASSERT_RAW(ExecutionOrder[2] == 3); // C最后执行

    TaskGraph.ShutDown();
    HK_LOG_INFO(ELogcat::Test, "测试2通过\n");
}

void TestEarlyDependencyCompletion()
{
    HK_LOG_INFO(ELogcat::Test, "=== 测试3: 依赖提前完成 ===");
    auto& TaskGraph = FTaskGraph::GetRef();
    TaskGraph.StartUp();

    std::atomic<int> Counter{0};

    // 创建任务A，立即执行
    auto TaskA = TaskGraph.Create(FString("TaskA"), EExecutorLabel::IO,
                                  [&Counter]()
                                  {
                                      Counter = 1;
                                      HK_LOG_INFO(ELogcat::Test, "TaskA执行完成");
                                  });

    TaskGraph.Launch(TaskA);
    TaskA->Wait(); // 等待A完成

    // 创建任务B，依赖已经完成的TaskA
    auto TaskB = TaskGraph.Create(
        FString("TaskB"), EExecutorLabel::IO,
        [&Counter]()
        {
            Counter = Counter.load() + 1;
            HK_LOG_INFO(ELogcat::Test, "TaskB执行完成，Counter = {}", Counter.load());
        },
        TaskA);

    HK_LOG_INFO(ELogcat::Test, "TaskA已完成，启动TaskB...");
    TaskGraph.Launch(TaskB);
    TaskB->Wait();

    HK_ASSERT_RAW(Counter.load() == 2);

    TaskGraph.ShutDown();
    HK_LOG_INFO(ELogcat::Test, "测试3通过\n");
}

void TestConcurrentTasks()
{
    HK_LOG_INFO(ELogcat::Test, "=== 测试4: 并发任务 ===");
    auto& TaskGraph = FTaskGraph::GetRef();
    TaskGraph.StartUp();

    std::atomic<int> CompletedCount{0};
    const int TaskCount = 10;

    TArray<TSharedPtr<FTask>> Tasks;
    for (int i = 0; i < TaskCount; ++i)
    {
        auto Task =
            TaskGraph.Create(FString("ConcurrentTask_") + FString(std::to_string(i).c_str()), EExecutorLabel::IO,
                             [&CompletedCount, i]()
                             {
                                 std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                 CompletedCount.fetch_add(1);
                                 HK_LOG_INFO(ELogcat::Test, "任务{}完成", i);
                             });
        Tasks.Add(Task);
        TaskGraph.Launch(Task);
    }

    // 等待所有任务完成
    for (auto& Task : Tasks)
    {
        Task->Wait();
    }

    HK_ASSERT_RAW(CompletedCount.load() == TaskCount);
    HK_LOG_INFO(ELogcat::Test, "所有{}个并发任务完成", TaskCount);

    TaskGraph.ShutDown();
    HK_LOG_INFO(ELogcat::Test, "测试4通过\n");
}

void TestDifferentExecutors()
{
    HK_LOG_INFO(ELogcat::Test, "=== 测试5: 不同Executor ===");
    auto& TaskGraph = FTaskGraph::GetRef();
    TaskGraph.StartUp();

    std::atomic<int> GameTaskCount{0};
    std::atomic<int> RenderTaskCount{0};
    std::atomic<int> IOTaskCount{0};

    // Game线程任务
    auto GameTask = TaskGraph.Create(FString("GameTask"), EExecutorLabel::Game,
                                     [&GameTaskCount]()
                                     {
                                         GameTaskCount = 1;
                                         HK_LOG_INFO(ELogcat::Test, "Game任务执行完成");
                                     });

    // Render线程任务
    auto RenderTask = TaskGraph.Create(FString("RenderTask"), EExecutorLabel::Render,
                                       [&RenderTaskCount]()
                                       {
                                           RenderTaskCount = 1;
                                           HK_LOG_INFO(ELogcat::Test, "Render任务执行完成");
                                       });

    // IO线程任务
    auto IOTask = TaskGraph.Create(FString("IOTask"), EExecutorLabel::IO,
                                   [&IOTaskCount]()
                                   {
                                       IOTaskCount = 1;
                                       HK_LOG_INFO(ELogcat::Test, "IO任务执行完成");
                                   });

    TaskGraph.Launch(GameTask);
    TaskGraph.Launch(RenderTask);
    TaskGraph.Launch(IOTask);

    // Game线程需要Tick
    while (!GameTask->IsCompleted())
    {
        TaskGraph.Tick();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // 等待所有任务完成
    RenderTask->Wait();
    IOTask->Wait();

    HK_ASSERT_RAW(GameTaskCount.load() == 1);
    HK_ASSERT_RAW(RenderTaskCount.load() == 1);
    HK_ASSERT_RAW(IOTaskCount.load() == 1);

    TaskGraph.ShutDown();
    HK_LOG_INFO(ELogcat::Test, "测试5通过\n");
}

void TestCreateAndLaunch()
{
    HK_LOG_INFO(ELogcat::Test, "=== 测试6: CreateAndLaunch ===");
    auto& TaskGraph = FTaskGraph::GetRef();
    TaskGraph.StartUp();

    std::atomic<int> Counter{0};

    // 使用CreateAndLaunch
    TaskGraph.Launch(FString("CreateAndLaunchTask"), EExecutorLabel::IO,
                     [&Counter]()
                     {
                         Counter = 100;
                         HK_LOG_INFO(ELogcat::Test, "CreateAndLaunch任务执行完成");
                     });

    // 等待任务完成（需要一些时间）
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    HK_ASSERT_RAW(Counter.load() == 100);

    TaskGraph.ShutDown();
    HK_LOG_INFO(ELogcat::Test, "测试6通过\n");
}

void TestComplexDependencies()
{
    HK_LOG_INFO(ELogcat::Test, "=== 测试7: 复杂依赖关系 ===");
    auto& TaskGraph = FTaskGraph::GetRef();
    TaskGraph.StartUp();

    TArray<int> Results;
    std::mutex ResultsMutex;

    // 创建多个任务，形成复杂的依赖图
    auto Task1 = TaskGraph.Create(FString("Task1"), EExecutorLabel::IO,
                                  [&Results, &ResultsMutex]()
                                  {
                                      std::lock_guard<std::mutex> Lock(ResultsMutex);
                                      Results.Add(1);
                                  });

    auto Task2 = TaskGraph.Create(
        FString("Task2"), EExecutorLabel::IO,
        [&Results, &ResultsMutex]()
        {
            std::lock_guard<std::mutex> Lock(ResultsMutex);
            Results.Add(2);
        },
        Task1);

    auto Task3 = TaskGraph.Create(
        FString("Task3"), EExecutorLabel::IO,
        [&Results, &ResultsMutex]()
        {
            std::lock_guard<std::mutex> Lock(ResultsMutex);
            Results.Add(3);
        },
        Task1);

    auto Task4 = TaskGraph.Create(
        FString("Task4"), EExecutorLabel::IO,
        [&Results, &ResultsMutex]()
        {
            std::lock_guard<std::mutex> Lock(ResultsMutex);
            Results.Add(4);
        },
        Task2, Task3);

    TaskGraph.Launch(Task1);
    TaskGraph.Launch(Task2);
    TaskGraph.Launch(Task3);
    TaskGraph.Launch(Task4);

    Task4->Wait();

    HK_ASSERT_RAW(Results.Size() == 4);
    HK_ASSERT_RAW(Results[0] == 1); // Task1先执行
    HK_ASSERT_RAW(Results[3] == 4); // Task4最后执行

    TaskGraph.ShutDown();
    HK_LOG_INFO(ELogcat::Test, "测试7通过\n");
}

int main()
{
#ifdef HK_WINDOWS
    // 设置控制台为UTF-8编码以支持中文显示
    SetConsoleOutputCP(65001); // UTF-8 code page
    SetConsoleCP(65001);       // UTF-8 code page
#endif

    HK_LOG_INFO(ELogcat::Test, "开始TaskGraph测试...\n");

    try
    {
        TestBasicTask();
        TestTaskDependencies();
        TestEarlyDependencyCompletion();
        TestConcurrentTasks();
        TestDifferentExecutors();
        TestCreateAndLaunch();
        TestComplexDependencies();

        HK_LOG_INFO(ELogcat::Test, "所有测试通过！");
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::Test, "测试失败: {}", e.what());
        return 1;
    }

    return 0;
}
