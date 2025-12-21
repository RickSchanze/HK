#pragma once
#include "Config/IConfig.h"
#include "Core/Serialization/Serialization.h"
#include "GfxDevice.h"
#include "Math/Vector.h"

#include "RHIConfig.generated.h"

HCLASS(ConfigPath = "Config/RHIConfig.xml")
class FRHIConfig : public IConfig
{
    GENERATED_BODY(FRHIConfig)

    HPROPERTY(DefaultProperty)
    FVector2i DefaultWindowSize = {1920, 1080};

    HPROPERTY(DefaultProperty)
    EGfxBackend GfxBackend = EGfxBackend::Vulkan;
};
