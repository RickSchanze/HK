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
    typedef void* FWindowHandle;

    FRHISurface Surface;
    FRHISwapChain SwapChain;
    FName WindowName;
    FWindowHandle Handle = nullptr;
    FVector2i Size;
    bool bIsOpened = false;

    void Open();
    void Close();
    void Destroy();

    bool IsOpened() const { return bIsOpened; }

    bool IsValid() const
    {
        return Surface.Handle.IsValid() && SwapChain.Handle.IsValid() && Handle != nullptr;
    }

    ~FRHIWindow();
};

class FRHIWindowManager : public FSingleton<FRHIWindowManager>
{
    friend class FGfxDevice;
public:


private:
    // Index = 0 代表MainWindow
    TUniquePtr<FRHIWindow> Windows[MAX_RHI_WINDOW_COUNT];
};
