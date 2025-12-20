#pragma once
#include "Config/IConfig.h"
#include "Math/Vector.h"
#include "Core/Serialization/Serialization.h"

#include "RHIConfig.generated.h"

HCLASS(ConfigPath = "Config/RHIConfig.xml")
class FRHIConfig : public IConfig
{
    GENERATED_BODY(FRHIConfig)
public:

private:
    HPROPERTY(DefaultProperty)
    FVector2i DefaultWindowSize = {1920, 1080};
};
