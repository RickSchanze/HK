#pragma once

#include "Config/IConfig.h"

HCLASS(ConfigPath = "Config/RenderConfig.xml")
class FRenderConfig : public IConfig
{
public:
    const TArray<FString>& GetShaderPaths() const
    {
        return ShaderPaths;
    }

private:
    HPROPERTY()
    TArray<FString> ShaderPaths;
};

