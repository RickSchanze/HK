#include "Core/Logging/Logger.h"
#include <filesystem>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

// 全局Logger实例
FLogger GLogger;

// FLogger实现
FLogger::FLogger()
{
    Initialize();
}

FLogger::~FLogger()
{
    if (MyLogger)
    {
        MyLogger->flush();
        spdlog::drop_all();
    }
}

void FLogger::Initialize()
{
    try
    {
        // 获取可执行文件所在目录
        std::filesystem::path ExePath;
#ifdef _WIN32
        char ExePathBuf[MAX_PATH];
        DWORD Length = GetModuleFileNameA(nullptr, ExePathBuf, MAX_PATH);
        if (Length > 0 && Length < MAX_PATH)
        {
            ExePath = ExePathBuf;
        }
#else
        char ExePathBuf[PATH_MAX];
        ssize_t Length = readlink("/proc/self/exe", ExePathBuf, PATH_MAX - 1);
        if (Length > 0)
        {
            ExePathBuf[Length] = '\0';
            ExePath = ExePathBuf;
        }
#endif
        
        // 如果无法获取exe路径，使用当前工作目录
        if (ExePath.empty())
        {
            ExePath = std::filesystem::current_path();
        }
        else
        {
            ExePath = ExePath.parent_path();
        }
        
        // 创建Logs目录（相对于exe位置）
        std::filesystem::path LogsDir = ExePath / "Logs";
        if (!std::filesystem::exists(LogsDir))
        {
            std::filesystem::create_directories(LogsDir);
        }

        // 创建多个sink
        std::vector<spdlog::sink_ptr> Sinks;

        // 控制台sink（彩色输出）
        auto ConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        ConsoleSink->set_level(spdlog::level::trace);
        ConsoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
        Sinks.push_back(ConsoleSink);

        // 文件sink（旋转日志，每个文件10MB，最多5个文件）
        std::filesystem::path LogFile = LogsDir / "HKEngine.log";
        auto FileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            LogFile.string(), 1024 * 1024 * 10, 5);
        FileSink->set_level(spdlog::level::trace);
        FileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
        Sinks.push_back(FileSink);

        // 创建logger（多线程安全）
        MyLogger = std::make_shared<spdlog::logger>("HKEngine", Sinks.begin(), Sinks.end());
        MyLogger->set_level(spdlog::level::trace);
        MyLogger->flush_on(spdlog::level::warn);

        // 注册为全局logger
        spdlog::register_logger(MyLogger);
        spdlog::set_default_logger(MyLogger);
    }
    catch (const std::exception& e)
    {
        // 如果初始化失败，使用默认logger
        MyLogger = spdlog::default_logger();
        MyLogger->error("Failed to initialize logger: {}", e.what());
    }
}


