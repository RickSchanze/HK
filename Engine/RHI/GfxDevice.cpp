//
// Created by Admin on 2025/12/15.
//

#include "GfxDevice.h"

#include "Core/Utility/Profiler.h"
#include "Vulkan/GfxDeviceVk.h"

static inline FGfxDevice* GGfxDevice = nullptr;

void CreateGfxDevice()
{
    GOnPreRHIDeviceCreated.Invoke();
    GGfxDevice = New<FGfxDeviceVk>();
    GGfxDevice->Init();
    GOnPostRHIDeviceCreated.Invoke(GGfxDevice);
}

void DestroyGfxDevice()
{
    GOnPreRHIDeviceDestroyed.Invoke(GGfxDevice);
    Delete(GGfxDevice);
    GGfxDevice = nullptr;
    GOnPostRHIDeviceDestroyed.Invoke();
}

FGfxDevice* GetGfxDevice()
{
    return GGfxDevice;
}