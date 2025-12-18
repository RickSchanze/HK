#pragma once

// Tracy性能分析器封装
// 根据HK_ENABLE_PROFILING宏定义启用/禁用性能分析功能

#if HK_ENABLE_PROFILING
// 启用性能分析
#include <cstddef>
#include <cstring>
#include <tracy/Tracy.hpp>
#include <utility>

// Zone作用域宏 - 自动跟踪函数作用域
#define HK_PROFILE_SCOPE() ZoneScoped

// Zone作用域宏 - 带名称
#define HK_PROFILE_SCOPE_N(Name) ZoneScopedN(Name)

// Zone文本 - 添加文本信息到当前Zone
#define HK_PROFILE_TEXT(Text) ZoneText(Text, strlen(Text))

// Zone文本 - 带长度
#define HK_PROFILE_TEXT_L(Text, Len) ZoneText(Text, Len)

// Zone值 - 添加数值信息到当前Zone
#define HK_PROFILE_VALUE(Value) ZoneValue(Value)

// Frame标记 - 标记一帧的结束
#define HK_PROFILE_FRAME_MARK() FrameMark

// Frame标记 - 带名称
#define HK_PROFILE_FRAME_MARK_N(Name) FrameMarkNamed(Name)

// 消息 - 发送消息到Tracy
#define HK_PROFILE_MESSAGE(Text) TracyMessage(Text, strlen(Text))

// 消息 - 带颜色
#define HK_PROFILE_MESSAGE_C(Text, Color) TracyMessageC(Text, strlen(Text), Color)

// Plot - 绘制数值
#define HK_PROFILE_PLOT(Name, Value) TracyPlot(Name, Value)

// Plot - 绘制浮点数值
#define HK_PROFILE_PLOT_F(Name, Value) TracyPlot(Name, static_cast<double>(Value))

// 内存分配跟踪
#define HK_PROFILE_ALLOC(Ptr, Size) TracyAlloc(Ptr, Size)

// 内存释放跟踪
#define HK_PROFILE_FREE(Ptr) TracyFree(Ptr)

// 设置线程名称
#define HK_PROFILE_SET_THREAD_NAME(Name) tracy::SetThreadName(Name)

// 内存分配函数模板 - 替代new操作符
template <typename T, typename... Args>
T* New(Args&&... args)
{
    // 使用标准的new，确保分配和释放匹配
    T* Obj = new T(std::forward<Args>(args)...);
    // TracyAlloc需要原始指针，Obj就是实际分配的指针
    TracyAlloc(Obj, sizeof(T));
    return Obj;
}

// 数组分配函数模板 - 替代new[]操作符
// 使用标准的new[]以确保正确的内存布局和析构函数调用
template <typename T>
T* NewArray(size_t Count)
{
    T* Array = new T[Count];
    // TracyAlloc需要原始指针，Array就是实际分配的指针
    // 注意：对于数组，实际分配的大小可能包含额外的元数据，但Tracy只需要知道用户可见的大小
    TracyAlloc(Array, sizeof(T) * Count);
    return Array;
}

// 内存释放函数模板 - 替代delete操作符
template <typename T>
void Delete(T* Ptr)
{
    if (Ptr != nullptr)
    {
        // TracyFree必须使用与TracyAlloc相同的指针
        TracyFree(Ptr);
        delete Ptr;
    }
}

// 数组释放函数模板 - 替代delete[]操作符
template <typename T>
void DeleteArray(T* Ptr)
{
    if (Ptr != nullptr)
    {
        // TracyFree必须使用与TracyAlloc相同的指针
        TracyFree(Ptr);
        delete[] Ptr;
    }
}

#else
// 禁用性能分析 - 所有宏都展开为空操作
#include <cstddef>
#include <utility>

#define HK_PROFILE_SCOPE() ((void)0)
#define HK_PROFILE_SCOPE_N(Name) ((void)0)
#define HK_PROFILE_TEXT(Text) ((void)0)
#define HK_PROFILE_TEXT_L(Text, Len) ((void)0)
#define HK_PROFILE_VALUE(Value) ((void)0)
#define HK_PROFILE_FRAME_MARK() ((void)0)
#define HK_PROFILE_FRAME_MARK_N(Name) ((void)0)
#define HK_PROFILE_MESSAGE(Text) ((void)0)
#define HK_PROFILE_MESSAGE_C(Text, Color) ((void)0)
#define HK_PROFILE_PLOT(Name, Value) ((void)0)
#define HK_PROFILE_PLOT_F(Name, Value) ((void)0)
#define HK_PROFILE_ALLOC(Ptr, Size) ((void)0)
#define HK_PROFILE_FREE(Ptr) ((void)0)
#define HK_PROFILE_SET_THREAD_NAME(Name) ((void)0)

// 禁用性能分析时的内存分配函数 - 直接使用标准new
template <typename T, typename... Args>
T* New(Args&&... args)
{
    return new T(std::forward<Args>(args)...);
}

template <typename T>
T* NewArray(size_t Count)
{
    return new T[Count];
}

template <typename T>
void Delete(T* Ptr)
{
    delete Ptr;
}

template <typename T>
void DeleteArray(T* Ptr)
{
    delete[] Ptr;
}

#endif // HK_ENABLE_PROFILING
