#pragma once

#include "Config/IConfig.h"

HCLASS(ConfigPath = "Config/RenderConfig.xml")
class FRenderConfig : public IConfig
{

private:
    HPROPERTY()
    TArray<FString> ShaderPaths;
};

