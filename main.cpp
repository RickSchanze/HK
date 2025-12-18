#ifdef HK_WINDOWS
#include <windows.h>
#endif
#include "Core/Logging/Logger.h"
#include "Loop/EngineLoop.h"

int main()
{
    HK_LOG_INFO(ELogcat::Engine, "HKEngine Booting...");
#ifdef HK_WINDOWS
    HK_LOG_INFO(ELogcat::Engine, "Set console output encoding to UTF8.");
    // 设置控制台为UTF-8编码以支持中文显示
    SetConsoleOutputCP(65001); // UTF-8 code page
    SetConsoleCP(65001);       // UTF-8 code page
#endif

    // 创建并初始化引擎循环
    FEngineLoop EngineLoop;

    // 设置渲染和输入Tick函数（示例）
    // 注意：这里设置为nullptr以触发警告，实际使用时应该设置实际的函数
    EngineLoop.SetRenderTickFunc(nullptr);
    EngineLoop.SetInputTickFunc(nullptr);

    // 初始化引擎循环
    EngineLoop.Init();

    // 运行引擎循环
    EngineLoop.Run();

    // 清理引擎循环
    EngineLoop.UnInit();

    HK_LOG_INFO(ELogcat::Engine, "HKEngine 退出");

    return 0;
}
