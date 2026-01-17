//
// Created by Admin on 2026/1/13.
//

#include "Shader.h"

#include "RHI/GfxDevice.h"

bool HShader::Compile(TFixedArray<FRHIShaderModule, 2>& OutShaderModules, bool ClearCode)
{
    if (!ShaderCompileResult.IsValid())
    {
        return false;
    }

    FRHIShaderModuleDesc Desc;
    Desc.Code           = ShaderCompileResult.VS;
    Desc.DebugName      = std::format("{}_VS", Name);
    OutShaderModules[0] = GetGfxDeviceRef().CreateShaderModule(Desc, ERHIShaderStage::Vertex);

    Desc.Code           = ShaderCompileResult.FS;
    Desc.DebugName      = std::format("{}_FS", Name);
    OutShaderModules[1] = GetGfxDeviceRef().CreateShaderModule(Desc, ERHIShaderStage::Fragment);
    if (ClearCode)
    {
        ShaderCompileResult.
    }
    return true;
}
