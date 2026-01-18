//
// Created by Admin on 2026/1/13.
//

#include "Material.h"
#include "Render/Shader/Shader.h"
#include "SharedMaterial.h"

HMaterial::HMaterial() : ParameterBlock(nullptr) {}

HMaterial::~HMaterial() {}

void HMaterial::BindTexture(const FName& InName, const HTexture* InTexture)
{
    if (InTexture == nullptr)
    {
        HK_LOG_ERROR(ELogcat::Render, "绑定材质 {} 纹理 {} 失败: 纹理为空", Name, InName);
        return;
    }
    if (!ParameterBlock.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Render, "绑定材质 {} 纹理 {} 失败: 参数块无效", Name, InName);
        return;
    }
    FMaterialParameter Parameter = ParameterBlock.GetParameter(InName);
    if (!Parameter.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Render, "绑定材质 {} 纹理 {} 失败: 参数无效", Name, InName);
        return;
    }

}

void HMaterial::BindModelMatrixIndex(const UInt32 InModelIndex) {}

FMaterialParameterBlock::FMaterialParameterBlock(const HShader* InShader)
{
    if (InShader == nullptr)
    {
        return;
    }
    const auto& ParameterSheet = InShader->GetCompileResult().ParameterSheet;
    if (!ParameterSheet.bIsValid)
    {
        return;
    }
    Parameters.Reserve(ParameterSheet.PushConstants.Size());
    for (const auto& PushConstant : ParameterSheet.PushConstants)
    {
        FMaterialParameter Parameter{PushConstant.Name, PushConstant.Offset, PushConstant.Size};
        Parameters.Add(Parameter);
    }
    Parameters.Sort([](const FMaterialParameter& A, const FMaterialParameter& B) { return A.Offset < B.Offset; });
    BlockSize = Parameters.Back().Offset + Parameters.Back().Size;
}