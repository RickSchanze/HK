//
// Created by Admin on 2026/1/13.
//

#include "Material.h"
#include "Render/GlobalRenderResources.h"
#include "Render/Shader/Shader.h"
#include "SharedMaterial.h"

HMaterial::HMaterial() : ParameterBlock(nullptr) {}

HMaterial::~HMaterial() {}

void HMaterial::BindTexture(const FName& InName, HTexture* InTexture)
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

    // 确保 ParameterBuffer 有足够的大小
    UInt32 RequiredSize = ParameterBlock.GetBlockSize();
    if (ParameterBuffer.Size() < RequiredSize)
    {
        ParameterBuffer.Resize(RequiredSize);
        ParameterBuffer.Fill(0); // 初始化为 0
    }

    // 使用 FGlobalStaticResourcePool 获取或添加纹理索引
    auto& ResourcePool = FGlobalStaticRenderResourcePool::GetRef();
    Int16 TextureIndex = ResourcePool.GetOrAddTextureIndex(InTexture);

    if (TextureIndex < 0)
    {
        HK_LOG_ERROR(ELogcat::Render, "绑定材质 {} 纹理 {} 失败: 无法获取纹理索引", Name, InName);
        return;
    }

    // 写入纹理索引（Int16）
    UInt16 IndexValue = static_cast<UInt16>(TextureIndex);
    if (!ParameterBuffer.Write(Parameter.Offset, IndexValue))
    {
        HK_LOG_ERROR(ELogcat::Render, "绑定材质 {} 纹理 {} 失败: 写入 ParameterBuffer 失败", Name, InName);
        return;
    }

    // 标记参数已更新
    SetParameterUpdateDirty(true);

    HK_LOG_INFO(ELogcat::Render, "绑定材质 {} 纹理 {} 成功: Index={}, Offset={}, Size={}", Name, InName, TextureIndex,
                Parameter.Offset, Parameter.Size);
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