//
// Created by Admin on 2025/12/15.
//

#include "GfxDevice.h"

#include "Config/ConfigManager.h"
#include "Core/Utility/Profiler.h"
#include "RHIConfig.h"
#include "Vulkan/GfxDeviceVk.h"

static inline FGfxDevice* GGfxDevice = nullptr;

void CreateGfxDevice()
{
    GOnPreRHIDeviceCreated.Invoke();
    const auto* Config = FConfigManager::GetRef().GetConfig<FRHIConfig>();
    switch (Config->GetGfxBackend())
    {
        case EGfxBackend::Vulkan:
            GGfxDevice = New<FGfxDeviceVk>();
            break;
        default:
            HK_LOG_FATAL(ELogcat::RHI, "Unsupported GfxBackend");
            throw std::runtime_error("Unsupported Gfx Backend");
    }
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