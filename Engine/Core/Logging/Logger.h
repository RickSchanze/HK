#pragma once

#include "Core/Event/Event.h"
#include "Core/Logging/LogDefine.h"
#include "Core/String/StringFormatter.h"
#include "Core/Time/Time.h"
#include <fmt/format.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <thread>
#include <vector>

class FLogger
{
public:
    FLogger();
    ~FLogger();

    TEvent<const FLogContent&> OnLog;

    // 使用spdlog的fmt进行格式化，支持编译期类型检查
    template <typename... Args>
    void Fatal(ELogcat InLogcat, fmt::format_string<Args...> Fmt, Args&&... args)
    {
        Log(ELogLevel::Fatal, InLogcat, Fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Info(ELogcat InLogcat, fmt::format_string<Args...> Fmt, Args&&... args)
    {
        Log(ELogLevel::Info, InLogcat, Fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Warn(ELogcat InLogcat, fmt::format_string<Args...> Fmt, Args&&... args)
    {
        Log(ELogLevel::Warning, InLogcat, Fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Error(ELogcat InLogcat, fmt::format_string<Args...> Fmt, Args&&... args)
    {
        Log(ELogLevel::Error, InLogcat, Fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Debug(ELogcat InLogcat, fmt::format_string<Args...> Fmt, Args&&... args)
    {
        Log(ELogLevel::Debug, InLogcat, Fmt, std::forward<Args>(args)...);
    }

private:
    void Initialize();
    template <typename... Args>
    void Log(ELogLevel InLevel, ELogcat InLogcat, fmt::format_string<Args...> Fmt, Args&&... args)
    {
        // 格式化消息
        std::string Message = fmt::format(Fmt, std::forward<Args>(args)...);

        // 创建日志内容
        FLogContent LogContent;
        LogContent.Level = InLevel;
        LogContent.Logcat = InLogcat;
        LogContent.Message = FString(Message);
        // 获取线程ID（转换为整数）
        auto ThreadID = std::this_thread::get_id();
        LogContent.ThreadID = static_cast<Int32>(std::hash<std::thread::id>{}(ThreadID));
        LogContent.Time = FTimePoint::Now();

        // 先调用OnLog事件
        OnLog.Invoke(LogContent);

        // 然后使用spdlog输出
        const char* LogcatStr = GetLogcatString(InLogcat);
        std::string FullMessage = fmt::format("[{}] {}", LogcatStr ? LogcatStr : "Unknown", Message);

        switch (InLevel)
        {
            case ELogLevel::Debug:
                MyLogger->debug(FullMessage);
                break;
            case ELogLevel::Info:
                MyLogger->info(FullMessage);
                break;
            case ELogLevel::Warning:
                MyLogger->warn(FullMessage);
                break;
            case ELogLevel::Error:
                MyLogger->error(FullMessage);
                break;
            case ELogLevel::Fatal:
                MyLogger->critical(FullMessage);
                break;
        }
    }

    std::shared_ptr<spdlog::logger> MyLogger;
};

extern FLogger GLogger;

// 日志宏
#define HK_LOG_FATAL(InLogcat, Fmt, ...) GLogger.Fatal(InLogcat, Fmt __VA_OPT__(, ) __VA_ARGS__)
#define HK_LOG_ERROR(InLogcat, Fmt, ...) GLogger.Error(InLogcat, Fmt __VA_OPT__(, ) __VA_ARGS__)
#define HK_LOG_WARN(InLogcat, Fmt, ...) GLogger.Warn(InLogcat, Fmt __VA_OPT__(, ) __VA_ARGS__)
#define HK_LOG_INFO(InLogcat, Fmt, ...) GLogger.Info(InLogcat, Fmt __VA_OPT__(, ) __VA_ARGS__)
#define HK_LOG_DEBUG(InLogcat, Fmt, ...) GLogger.Debug(InLogcat, Fmt __VA_OPT__(, ) __VA_ARGS__)

// Assert宏（在Debug模式下使用Logger）
#ifdef HK_DEBUG
#define HK_ASSERT(Condition)                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(Condition))                                                                                              \
        {                                                                                                              \
            GLogger.Fatal(ELogcat::Assert, "Assertion failed: {}", #Condition);                                        \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (0)

#define HK_ASSERT_MSG(Condition, Fmt, ...)                                                                             \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(Condition))                                                                                              \
        {                                                                                                              \
            GLogger.Fatal(ELogcat::Assert, "Assertion failed: {} - " Fmt, #Condition __VA_OPT__(, ) __VA_ARGS__);      \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (0)
#else
#define HK_ASSERT(Condition) ((void)0)
#define HK_ASSERT_MSG(Condition, Fmt, ...) ((void)0)
#endif
