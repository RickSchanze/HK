#pragma once
#include "Core/Event/Event.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Singleton/Singleton.h"

struct FRHIBufferCreateInfo;
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
    virtual ~FGfxDevice() = default;
    // 创建缓冲区，返回的值类型包含一个 Handle
    // 可以像普通值类型一样拷贝和移动
    // 只有通过 CreateBuffer 创建的 Buffer 才是有效的
    virtual FRHIBuffer CreateBuffer(const FRHIBufferCreateInfo& BufferCreateInfo) = 0;

    // 销毁缓冲区资源
    // 必须通过此方法销毁，不能直接调用 Buffer.Destroy()
    virtual void DestroyBuffer(FRHIBuffer& Buffer) = 0;
};

inline TEvent<> GOnPreRHIDeviceCreated;
inline TEvent<FGfxDevice*> GOnPostRHIDeviceCreated;
inline TEvent<FGfxDevice*> GOnPreRHIDeviceDestroyed;
inline TEvent<FGfxDevice*> GOnPostRHIDeviceDestroyed;

HK_API void CreateRHIDevice();
HK_API void DestroyRHIDevice();

HK_API FGfxDevice* GetRHIDevice();
