//
// Created by Admin on 2025/12/17.
//

#include "RHIWindow.h"
#include "Core/Logging/Logger.h"
#include "Core/Utility/Profiler.h"
#include "Core/Utility/UniquePtr.h"
#include "Loop/LoopData.h"
#include "RHI/GfxDevice.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

/**
 * 根据 SDL WindowID 查找对应的 FRHIWindow
 * @param WindowID SDL 窗口 ID
 * @param Windows 窗口数组
 * @return 找到的窗口指针, 失败返回 nullptr
 */
static FRHIWindow* FindWindowByID(const Uint32 WindowID, TUniquePtr<FRHIWindow>* Windows)
{
    for (int i = 0; i < MAX_RHI_WINDOW_COUNT; ++i)
    {
        if (Windows[i] && Windows[i]->IsValid())
        {
            if (auto* SDLWindow = static_cast<SDL_Window*>(Windows[i]->GetHandle());
                SDLWindow && SDL_GetWindowID(SDLWindow) == WindowID)
            {
                return Windows[i].Get();
            }
        }
    }
    return nullptr;
}

/**
 * 处理窗口关闭请求
 * @param Window 要关闭的窗口
 * @param IsMainWindow 是否是主窗口
 */
static void HandleWindowCloseRequest(FRHIWindow* Window, const bool IsMainWindow)
{
    if (IsMainWindow)
    {
        HK_LOG_INFO(ELogcat::RHI, "主窗口 '{}' 收到关闭请求, 即将退出引擎...",
                    Window->GetWindowName().GetString().CStr());
        GLoopData.bShouldCloseEngine = true;
    }
    else
    {
        HK_LOG_INFO(ELogcat::RHI, "窗口 '{}' 收到关闭请求, 关闭窗口", Window->GetWindowName().GetString().CStr());
        if (FGfxDevice* GfxDevice = GetGfxDevice())
        {
            GfxDevice->CloseWindow(*Window);
        }
    }
}

/**
 * 处理窗口大小改变
 * @param Window 要更新的窗口
 */
void HandleWindowResize(FRHIWindow* Window)
{
    if (auto* SDLWindow = static_cast<SDL_Window*>(Window->GetHandle()))
    {
        int Width, Height;
        SDL_GetWindowSize(SDLWindow, &Width, &Height);
        Window->SetSize(FVector2i(Width, Height));
        HK_LOG_DEBUG(ELogcat::RHI, "窗口 '{}' 大小改变: {}x{}", Window->GetWindowName().GetString().CStr(), Width,
                     Height);
    }
}

/**
 * 处理键盘事件
 * @param Event SDL 键盘事件
 */
void HandleKeyEvent(const SDL_Event& Event)
{
    // bool bPressed = (Event.type == SDL_EVENT_KEY_DOWN);
    // SDL_Keycode Key = Event.key.key;
}

static void HandleSDLEvent(const SDL_Event& Event, TUniquePtr<FRHIWindow>* Windows)
{
    // 根据事件类型处理
    switch (Event.type)
    {
        case SDL_EVENT_QUIT:
            // 应用程序退出事件
            HK_LOG_INFO(ELogcat::RHI, "收到 SDL_QUIT 事件, 即将退出引擎...");
            GLoopData.bShouldCloseEngine = true;
            break;

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            // 窗口关闭请求
            {
                if (FRHIWindow* Window = FindWindowByID(Event.window.windowID, Windows))
                {
                    // 检查是否是主窗口（通过在 Windows 数组中的索引判断）
                    bool IsMainWindow = false;
                    for (int i = 0; i < MAX_RHI_WINDOW_COUNT; ++i)
                    {
                        if (Windows[i].Get() == Window)
                        {
                            IsMainWindow = (i == 0);
                            break;
                        }
                    }
                    HandleWindowCloseRequest(Window, IsMainWindow);
                }
            }
            break;

        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            // 窗口大小改变
            {
                if (FRHIWindow* Window = FindWindowByID(Event.window.windowID, Windows))
                {
                    HandleWindowResize(Window);
                }
            }
            break;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            // 键盘事件
            HandleKeyEvent(Event);
            break;

        default:
            // 其他事件类型可以在这里添加处理
            break;
    }
}

void FRHIWindowManager::PollAllWindowInput()
{
    HK_PROFILE_SCOPE();
    // 处理所有 SDL 事件
    SDL_Event Event;
    SDL_PollEvent(&Event);
    HandleSDLEvent(Event, GetRef().Windows);
}

void FRHIWindow::Open()
{
    if (bIsOpened)
    {
        HK_LOG_WARN(ELogcat::RHI, "窗口已经打开: {}", WindowName.GetString().CStr());
        return;
    }

    // 通过 FGfxDevice 打开窗口
    if (FGfxDevice* GfxDevice = GetGfxDevice())
    {
        try
        {
            GfxDevice->OpenWindow(*this);
        }
        catch (const std::exception& e)
        {
            HK_LOG_ERROR(ELogcat::RHI, "打开窗口失败: {}", e.what());
        }
    }
    else
    {
        HK_LOG_ERROR(ELogcat::RHI, "GfxDevice未初始化, 无法打开窗口");
    }
}

void FRHIWindow::Close()
{
    if (!bIsOpened)
    {
        HK_LOG_WARN(ELogcat::RHI, "窗口已经关闭: {}", WindowName.GetString().CStr());
        return;
    }

    // 通过 FGfxDevice 关闭窗口
    if (FGfxDevice* GfxDevice = GetGfxDevice())
    {
        try
        {
            GfxDevice->CloseWindow(*this);
        }
        catch (const std::exception& e)
        {
            HK_LOG_ERROR(ELogcat::RHI, "关闭窗口失败: {}", e.what());
        }
    }
    else
    {
        HK_LOG_ERROR(ELogcat::RHI, "GfxDevice未初始化, 无法关闭窗口");
    }
}

FRHIWindow::~FRHIWindow()
{
    if (IsOpened())
    {
        HK_LOG_FATAL(ELogcat::RHI, "窗口销毁时未关闭: {}", WindowName.GetString().CStr());
    }
}

FRHIWindow* FRHIWindowManager::CreateRHIWindow(const FName Name, const FVector2i Size)
{
    // 查找空闲的窗口槽位（从1开始, 0是主窗口）
    Int32 FreeIndex = -1;
    for (Int32 i = 1; i < MAX_RHI_WINDOW_COUNT; ++i)
    {
        if (!Windows[i])
        {
            FreeIndex = i;
            break;
        }
    }

    if (FreeIndex < 0)
    {
        HK_LOG_ERROR(ELogcat::RHI, "窗口数量已达上限: {}", MAX_RHI_WINDOW_COUNT);
        return nullptr;
    }

    // 检查是否已存在同名窗口
    for (Int32 i = 1; i < MAX_RHI_WINDOW_COUNT; ++i)
    {
        if (Windows[i] && Windows[i]->GetWindowName() == Name)
        {
            HK_LOG_WARN(ELogcat::RHI, "窗口名称已存在: {}", Name.GetString().CStr());
            return nullptr;
        }
    }

    // 创建窗口对象
    Windows[FreeIndex] = MakeUnique<FRHIWindow>();
    FRHIWindow* Window = Windows[FreeIndex].Get();
    Window->SetWindowName(Name);
    Window->SetSize(Size);

    // 通过 FGfxDevice 创建实际的窗口资源（Surface、SwapChain等）
    if (FGfxDevice* GfxDevice = GetGfxDevice())
    {
        try
        {
            GfxDevice->CreateRHIWindow(Name, Size, *Window);
            HK_LOG_INFO(ELogcat::RHI, "窗口创建成功: {} (索引: {})", Name.GetString().CStr(), FreeIndex);
            return Window;
        }
        catch (const std::exception& e)
        {
            Windows[FreeIndex].Reset();
            HK_LOG_ERROR(ELogcat::RHI, "窗口创建失败: {}", e.what());
            return nullptr;
        }
    }
    else
    {
        Windows[FreeIndex].Reset();
        HK_LOG_ERROR(ELogcat::RHI, "GfxDevice未初始化");
        return nullptr;
    }
}

bool FRHIWindowManager::DestroyRHIWindow(const FName& Name)
{
    // 查找指定名称的窗口
    for (Int32 i = 1; i < MAX_RHI_WINDOW_COUNT; ++i)
    {
        if (Windows[i] && Windows[i]->GetWindowName() == Name)
        {
            return DestroyRHIWindow(Windows[i].Get());
        }
    }

    HK_LOG_WARN(ELogcat::RHI, "未找到指定名称的窗口: {}", Name.GetString().CStr());
    return false;
}

bool FRHIWindowManager::DestroyRHIWindow(FRHIWindow* Window)
{
    if (!Window)
    {
        HK_LOG_WARN(ELogcat::RHI, "窗口指针为空");
        return false;
    }

    // 查找窗口在数组中的位置
    for (Int32 i = 1; i < MAX_RHI_WINDOW_COUNT; ++i)
    {
        if (Windows[i] && Windows[i].Get() == Window)
        {
            // 通过 FGfxDevice 销毁窗口资源
            if (FGfxDevice* GfxDevice = GetGfxDevice())
            {
                try
                {
                    GfxDevice->DestroyRHIWindow(*Window);
                }
                catch (const std::exception& e)
                {
                    HK_LOG_ERROR(ELogcat::RHI, "窗口销毁失败: {}", e.what());
                    return false;
                }
            }

            // 清理窗口对象
            Windows[i].Reset();
            HK_LOG_INFO(ELogcat::RHI, "窗口已销毁 (索引: {})", i);
            return true;
        }
    }

    HK_LOG_WARN(ELogcat::RHI, "未找到要销毁的窗口");
    return false;
}
