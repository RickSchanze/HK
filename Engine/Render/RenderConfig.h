#pragma once

#include "Config/IConfig.h"

#include "RenderConfig.generated.h"

HCLASS(ConfigPath = "Config/RenderConfig.xml")
class FRenderConfig : public IConfig
{
    GENERATED_BODY(FRenderConfig)
public:
    const TArray<FString>& GetShaderPaths() const
    {
        return ShaderPaths;
    }

private:
    HPROPERTY()
    TArray<FString> ShaderPaths;
};

