#pragma once
#include "Core/String/Name.h"
#include "Core/Utility/UniquePtr.h"
#include "Math/Vector.h"
#include "RHIHandle.h"

#define MAX_RHI_WINDOW_COUNT 32

struct FRHISurface
{
    FRHIHandle Handle;
};

struct FRHISwapChain
{
    FRHIHandle Handle;
};

class FRHIWindow
{
    friend class FGfxDevice;
    typedef void* FWindowHandle;

public:
    // Get方法
    const FRHISurface& GetSurface() const
    {
        return Surface;
    }
    FRHISurface& GetSurface()
    {
        return Surface;
    }

    const FRHISwapChain& GetSwapChain() const
    {
        return SwapChain;
    }
    FRHISwapChain& GetSwapChain()
    {
        return SwapChain;
    }

    const FName& GetWindowName() const
    {
        return WindowName;
    }
    FName& GetWindowName()
    {
        return WindowName;
    }

    FWindowHandle GetHandle() const
    {
        return Handle;
    }
    FWindowHandle& GetHandle()
    {
        return Handle;
    }

    const FVector2i& GetSize() const
    {
        return Size;
    }
    FVector2i& GetSize()
    {
        return Size;
    }

    bool IsOpened() const
    {
        return bIsOpened;
    }

    bool IsValid() const
    {
        return Surface.Handle.IsValid() && SwapChain.Handle.IsValid() && Handle != nullptr;
    }

    // Set方法
    void SetSurface(const FRHISurface& InSurface)
    {
        Surface = InSurface;
    }
    void SetSwapChain(const FRHISwapChain& InSwapChain)
    {
        SwapChain = InSwapChain;
    }
    void SetWindowName(const FName& InWindowName)
    {
        WindowName = InWindowName;
    }
    void SetHandle(FWindowHandle InHandle)
    {
        Handle = InHandle;
    }
    void SetSize(const FVector2i& InSize)
    {
        Size = InSize;
    }
    void SetOpened(bool bOpened)
    {
        bIsOpened = bOpened;
    }

    void Open();
    void Close();

    // Destroy请调用FRHIWindowManager::Destroy
    // void Destroy();

private:
    ~FRHIWindow();

    FRHISurface Surface;
    FRHISwapChain SwapChain;
    FName WindowName;
    FWindowHandle Handle = nullptr;
    FVector2i Size;
    bool bIsOpened = false;
};

class FRHIWindowManager : public FSingleton<FRHIWindowManager>
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    /// 创建窗口
    /// @param Name 窗口名称
    /// @param Size 窗口大小
    /// @return 创建的窗口指针，失败返回nullptr
    FRHIWindow* CreateWindow(FName Name, FVector2i Size);

    /// 销毁窗口（通过名称）
    /// @param Name 窗口名称
    /// @return 是否成功销毁
    bool DestroyWindow(const FName& Name);

    /// 销毁窗口（通过指针）
    /// @param Window 窗口指针
    /// @return 是否成功销毁
    bool DestroyWindow(FRHIWindow* Window);

private:
    // Index = 0 代表MainWindow, 为Nullptr时代表未被占用
    TUniquePtr<FRHIWindow> Windows[MAX_RHI_WINDOW_COUNT];
};
