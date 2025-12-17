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
#include "Math/Vector.h"
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

void TestVector()
{
    HK_LOG_INFO(ELogcat::Test, "=== 测试8: Vector类型 ===");

    // 测试 Vector2
    {
        HK_LOG_INFO(ELogcat::Test, "--- 测试 FVector2f ---");
        FVector2f v1(3.0f, 4.0f);
        FVector2f v2(1.0f, 2.0f);

        // 加法
        FVector2f v3 = v1 + v2;
        HK_LOG_INFO(ELogcat::Test, "v1 + v2 = ({}, {})", v3.X, v3.Y);
        HK_ASSERT_RAW(v3.X == 4.0f && v3.Y == 6.0f);

        // 减法
        FVector2f v4 = v1 - v2;
        HK_LOG_INFO(ELogcat::Test, "v1 - v2 = ({}, {})", v4.X, v4.Y);
        HK_ASSERT_RAW(v4.X == 2.0f && v4.Y == 2.0f);

        // 标量乘法
        FVector2f v5 = v1 * 2.0f;
        HK_LOG_INFO(ELogcat::Test, "v1 * 2 = ({}, {})", v5.X, v5.Y);
        HK_ASSERT_RAW(v5.X == 6.0f && v5.Y == 8.0f);

        // 标量除法
        FVector2f v6 = v1 / 2.0f;
        HK_LOG_INFO(ELogcat::Test, "v1 / 2 = ({}, {})", v6.X, v6.Y);
        HK_ASSERT_RAW(v6.X == 1.5f && v6.Y == 2.0f);

        // 长度
        float length = v1.Length();
        HK_LOG_INFO(ELogcat::Test, "Length of v1 = {}", length);
        HK_ASSERT_RAW(std::abs(length - 5.0f) < 0.001f);

        // 归一化
        FVector2f v7 = v1.Normalized();
        HK_LOG_INFO(ELogcat::Test, "Normalized v1 = ({}, {})", v7.X, v7.Y);
        float normalizedLength = v7.Length();
        HK_ASSERT_RAW(std::abs(normalizedLength - 1.0f) < 0.001f);

        // 比较
        FVector2f v8(3.0f, 4.0f);
        HK_ASSERT_RAW(v1 == v8);
        HK_ASSERT_RAW(v1 != v2);
    }

    // 测试 Vector3
    {
        HK_LOG_INFO(ELogcat::Test, "--- 测试 FVector3f ---");
        FVector3f v1(1.0f, 2.0f, 3.0f);
        FVector3f v2(4.0f, 5.0f, 6.0f);

        // 加法
        FVector3f v3 = v1 + v2;
        HK_LOG_INFO(ELogcat::Test, "v1 + v2 = ({}, {}, {})", v3.X, v3.Y, v3.Z);
        HK_ASSERT_RAW(v3.X == 5.0f && v3.Y == 7.0f && v3.Z == 9.0f);

        // 复合赋值
        FVector3f v4 = v1;
        v4 += v2;
        HK_ASSERT_RAW(v4 == v3);

        // 长度平方
        float lengthSq = v1.LengthSquared();
        HK_LOG_INFO(ELogcat::Test, "LengthSquared of v1 = {}", lengthSq);
        HK_ASSERT_RAW(lengthSq == 14.0f);

        // 负号
        FVector3f v5 = -v1;
        HK_LOG_INFO(ELogcat::Test, "-v1 = ({}, {}, {})", v5.X, v5.Y, v5.Z);
        HK_ASSERT_RAW(v5.X == -1.0f && v5.Y == -2.0f && v5.Z == -3.0f);
    }

    // 测试 Vector4
    {
        HK_LOG_INFO(ELogcat::Test, "--- 测试 FVector4f ---");
        FVector4f v1(1.0f, 0.0f, 0.0f, 1.0f);
        FVector4f v2(0.0f, 1.0f, 0.0f, 1.0f);

        // 分量乘法
        FVector4f v3 = v1 * v2;
        HK_LOG_INFO(ELogcat::Test, "v1 * v2 (component-wise) = ({}, {}, {}, {})", v3.X, v3.Y, v3.Z, v3.W);
        HK_ASSERT_RAW(v3.X == 0.0f && v3.Y == 0.0f && v3.Z == 0.0f && v3.W == 1.0f);

        // 标量左乘
        FVector4f v4 = 2.0f * v1;
        HK_ASSERT_RAW(v4.X == 2.0f && v4.Y == 0.0f && v4.Z == 0.0f && v4.W == 2.0f);
    }

    // 测试整数向量
    {
        HK_LOG_INFO(ELogcat::Test, "--- 测试 FVector3i ---");
        FVector3i v1(10, 20, 30);
        FVector3i v2(5, 5, 5);

        FVector3i v3 = v1 - v2;
        HK_LOG_INFO(ELogcat::Test, "v1 - v2 = ({}, {}, {})", v3.X, v3.Y, v3.Z);
        HK_ASSERT_RAW(v3.X == 5 && v3.Y == 15 && v3.Z == 25);

        // 长度（返回 double）
        double length = v1.Length();
        HK_LOG_INFO(ELogcat::Test, "Length of integer vector = {}", length);
        HK_ASSERT_RAW(length > 0.0);
    }

    HK_LOG_INFO(ELogcat::Test, "测试8通过\n");
}

int main()
{
#ifdef HK_WINDOWS
    // 设置控制台为UTF-8编码以支持中文显示
    SetConsoleOutputCP(65001); // UTF-8 code page
    SetConsoleCP(65001);       // UTF-8 code page
#endif

    HK_LOG_INFO(ELogcat::Test, "开始测试...\n");

    try
    {
        // Vector测试
        TestVector();
        
        // TaskGraph测试
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
