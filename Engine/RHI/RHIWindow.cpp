//
// Created by Admin on 2025/12/17.
//

#include "RHIWindow.h"
#include "RHI/GfxDevice.h"
#include "Core/Logging/Logger.h"
#include "Core/Utility/UniquePtr.h"

void FRHIWindow::Open()
{
    if (bIsOpened)
    {
        HK_LOG_WARN(ELogcat::RHI, "窗口已经打开: {}", WindowName.GetString().CStr());
        return;
    }

    // 通过 FGfxDevice 打开窗口
    if (FGfxDevice* GfxDevice = GetRHIDevice())
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
        HK_LOG_ERROR(ELogcat::RHI, "GfxDevice未初始化，无法打开窗口");
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
    if (FGfxDevice* GfxDevice = GetRHIDevice())
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
        HK_LOG_ERROR(ELogcat::RHI, "GfxDevice未初始化，无法关闭窗口");
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
    // 查找空闲的窗口槽位（从1开始，0是主窗口）
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
    if (FGfxDevice* GfxDevice = GetRHIDevice())
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
            if (FGfxDevice* GfxDevice = GetRHIDevice())
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