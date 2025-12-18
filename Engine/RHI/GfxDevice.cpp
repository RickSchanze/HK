//
// Created by Admin on 2025/12/15.
//

#include "GfxDevice.h"

#include "Core/Utility/Profiler.h"
#include "Vulkan/GfxDeviceVk.h"

static inline FGfxDevice* GGfxDevice = nullptr;

void CreateRHIDevice()
{
    GOnPreRHIDeviceCreated.Invoke();
    GGfxDevice = New<FGfxDeviceVk>();
    GGfxDevice->Initialize();
    GOnPostRHIDeviceCreated.Invoke(GGfxDevice);
}

void DestroyRHIDevice()
{
    GOnPreRHIDeviceDestroyed.Invoke(GGfxDevice);
    Delete(GGfxDevice);
    GGfxDevice = nullptr;
    GOnPostRHIDeviceDestroyed.Invoke();
}

FGfxDevice* GetRHIDevice()
{
    return GGfxDevice;
}