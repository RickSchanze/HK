#pragma once
#include "IConfig.h"

#include "EngineConfig.generated.h"

HCLASS(ConfigPath = "Config/Engine.xml")
class HK_API FEngineConfig : public IConfig
{
    GENERATED_BODY(FEngineConfig)

    HPROPERTY(DefaultProperty)
    Int32 DefaultObjectCount = 1024;

    HPROPERTY(DefaultProperty)
    Int32 DefaultObjectIncreaseCount = 1024;
};
