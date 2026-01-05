#include "RHISync.h"
#include "Core/Logging/Logger.h"
#include "GfxDevice.h"

// ============================================================================
// FRHIFence 方法实现
// ============================================================================

bool FRHIFence::Wait(const UInt64 Timeout) const
{
    if (!IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Cannot wait for invalid fence");
        return false;
    }

    if (FGfxDevice* Device = GetGfxDevice())
    {
        return Device->WaitForFence(*this, Timeout);
    }

    HK_LOG_ERROR(ELogcat::RHI, "Failed to wait for fence: GfxDevice is null");
    return false;
}

bool FRHIFence::IsSignaled() const
{
    if (!IsValid())
    {
        return false;
    }

    if (const FGfxDevice* Device = GetGfxDevice())
    {
        return Device->IsFenceSignaled(*this);
    }

    return false;
}

bool FRHIFence::Reset() const
{
    if (!IsValid())
    {
        HK_LOG_ERROR(ELogcat::RHI, "Cannot reset invalid fence");
        return false;
    }

    if (FGfxDevice* Device = GetGfxDevice())
    {
        return Device->ResetFence(*this);
    }

    HK_LOG_ERROR(ELogcat::RHI, "Failed to reset fence: GfxDevice is null");
    return false;
}

// ============================================================================
// FScopedRHIFence 实现
// ============================================================================

FScopedRHIFence::FScopedRHIFence()
{
    Fence = GetGfxDeviceRef().CreateFence({});
}

FScopedRHIFence::~FScopedRHIFence()
{
    Fence.Wait();
    GetGfxDeviceRef().DestroyFence(Fence);
}
