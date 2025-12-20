#pragma once

#include "Core/Event/Event.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Utility/Macros.h"
#include "Math/Vector.h"

#include "GfxDevice.generated.h"

class FRHIWindow;
struct FRHIBufferDesc;
class FRHIBuffer;

HENUM()
enum class EGfxBackend
{
    Vulkan,
    D3D12,
    Metal,
    GL,
    GLES,
    Count,
};

class HK_API FGfxDevice
{
public:
    virtual void Init() = 0;
    virtual void UnInit() = 0;

    virtual ~FGfxDevice() = default;

#pragma region Buffer操作
    // 创建缓冲区，返回的值类型包含一个 Handle
    // 可以像普通值类型一样拷贝和移动
    // 只有通过 CreateBuffer 创建的 Buffer 才是有效的
    virtual FRHIBuffer CreateBuffer(const FRHIBufferDesc& BufferCreateInfo) = 0;

    // 销毁缓冲区资源
    // 必须通过此方法销毁，不能直接调用 Buffer.Destroy()
    virtual void DestroyBuffer(FRHIBuffer& Buffer) = 0;

    // 映射缓冲区内存（用于 CPU 访问）
    // @param Buffer 要映射的缓冲区
    // @param Offset 映射的偏移量（字节）
    // @param Size 映射的大小（字节），0 表示映射到缓冲区末尾
    // @return 映射的内存指针
    virtual void* MapBuffer(FRHIBuffer& Buffer, UInt64 Offset, UInt64 Size) = 0;

    // 取消映射缓冲区内存
    // @param Buffer 要取消映射的缓冲区
    virtual void UnmapBuffer(FRHIBuffer& Buffer) = 0;
#pragma endregion

#pragma region "窗口操作"
    // 窗口创建相关的函数, 这块的流程是这样的
    // 以Vulkan为例, Vulkan初始化Instance -> 拿Instance初始化MainWindow
    // 为MainWindow创建Surface -> Vulkan利用此Surface创建Device
    // 利用Device创建MainWindow的SwapChain
    // 销毁时先销毁SwapChain再销毁Surface, 这一点主窗口和其他一致
    /**
     * 创建主窗口的Surface
     * @param MainWindowName
     * @param MainWindowInitSize
     * @param OutMainWindow
     */
    virtual void CreateMainWindowSurface(FName MainWindowName, FVector2i MainWindowInitSize,
                                         FRHIWindow& OutMainWindow) = 0;

    /**
     * 创建主窗口的SwapChain
     * @param OutMainWindow
     */
    virtual void CreateMainWindowSwapChain(FRHIWindow& OutMainWindow) = 0;

    /**
     * 创建一个窗口
     * @param Name
     * @param Size
     * @param OutWindow
     */
    virtual void CreateRHIWindow(FName Name, FVector2i Size, FRHIWindow& OutWindow) = 0;

    /**
     * 销毁主窗口
     * @param MainWindow
     */
    virtual void DestroyMainWindow(FRHIWindow& MainWindow) = 0;

    /**
     * 销毁一个窗口
     * @param Window
     */
    virtual void DestroyRHIWindow(FRHIWindow& Window) = 0;

    /**
     * 打开窗口（显示窗口）
     * @param Window
     */
    virtual void OpenWindow(FRHIWindow& Window) = 0;

    /**
     * 关闭窗口（隐藏窗口）
     * @param Window
     */
    virtual void CloseWindow(FRHIWindow& Window) = 0;
#pragma endregion
};

inline TEvent<> GOnPreRHIDeviceCreated;
inline TEvent<FGfxDevice*> GOnPostRHIDeviceCreated;
inline TEvent<FGfxDevice*> GOnPreRHIDeviceDestroyed;
inline TEvent<> GOnPostRHIDeviceDestroyed;

HK_API void CreateGfxDevice();
HK_API void DestroyGfxDevice();

HK_API FGfxDevice* GetGfxDevice();
