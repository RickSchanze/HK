//
// Created by Admin on 2026/1/18.
//

#include "GlobalRenderResources.h"

#include "RHI/GfxDevice.h"
#include "Render/Material/SharedMaterial.h"
#include "Texture/Texture.h"

Int16 FGlobalStaticResourcePool::FindEmptyTextureIndex()
{
    for (int Index = 0; Index < TextureArray.Size(); Index++)
    {
        if (TextureArray[Index] == nullptr)
        {
            return Index;
        }
    }
    return -1;
}

Int16 FGlobalStaticResourcePool::FindEmptySamplerIndex()
{
    for (int Index = 0; Index < SamplerArray.Size(); Index++)
    {
        if (!SamplerArray[Index].IsValid())
        {
            return Index;
        }
    }
    return -1;
}

void FGlobalStaticResourcePool::AddTexture(HTexture* InTexture)
{
    if (InTexture == nullptr)
    {
        return;
    }
    if (TextureIndexMap.Contains(InTexture))
    {
        return;
    }

    // 检查纹理是否有效
    if (!InTexture->GetRHIImageView().IsValid())
    {
        return;
    }

    // 找到空槽位
    Int16 Index = FindEmptyTextureIndex();
    if (Index < 0)
    {
        HK_LOG_ERROR(ELogcat::Render, "纹理池已满，无法添加更多纹理");
        return;
    }

    // 将纹理添加到数组中
    TextureArray[Index] = InTexture;
    TextureIndexMap.Add(InTexture, Index);

    // 获取描述符集
    FRHIDescriptorSet StaticResourceDescriptorSet =
        FRHIPipelineResourcePool::GetRef().RequestCommonDescriptorSet(ECommonDescriptorSetIndex::StaticResource);
    auto& GfxDevice = GetGfxDeviceRef();

    // 更新描述符集（binding 0 是 SampledImage）
    FRHIWriteDescriptorSet WriteDesc{};
    WriteDesc.DstBinding      = 0;
    WriteDesc.DstArrayElement = static_cast<UInt32>(Index);
    WriteDesc.DescriptorCount = 1;
    WriteDesc.DescriptorType  = ERHIDescriptorType::SampledImage;

    FRHIDescriptorImageInfo ImageInfo{};
    ImageInfo.ImageView   = InTexture->GetRHIImageView();
    ImageInfo.ImageLayout = ERHIImageLayout::ShaderReadOnlyOptimal;
    WriteDesc.ImageInfo.Add(ImageInfo);

    TArray<FRHIWriteDescriptorSet> WriteDescriptorSets;
    WriteDescriptorSets.Add(WriteDesc);
    GfxDevice.UpdateDescriptorSet(StaticResourceDescriptorSet, WriteDescriptorSets);

    // 注册到 PreDestroyEvent 来移除绑定
    InTexture->GetPreDestroyEvent().AddBind(this, &FGlobalStaticResourcePool::RemoveTexture);

    HK_LOG_INFO(ELogcat::Render, "纹理已添加到纹理池: Index={}", Index);
}

void FGlobalStaticResourcePool::RemoveTexture(HTexture* InTexture)
{
    if (InTexture == nullptr)
    {
        return;
    }

    // 查找纹理索引
    Int16* IndexPtr = TextureIndexMap.Find(InTexture);
    if (IndexPtr == nullptr)
    {
        return;
    }

    Int16 Index = *IndexPtr;

    // 从数组中移除
    TextureArray[Index] = nullptr;
    TextureIndexMap.Remove(InTexture);

    // 获取描述符集
    FRHIDescriptorSet StaticResourceDescriptorSet =
        FRHIPipelineResourcePool::GetRef().RequestCommonDescriptorSet(ECommonDescriptorSetIndex::StaticResource);

    // 更新描述符集，将对应槽位设为空（使用无效的 ImageView）
    // 注意：Vulkan 不允许直接"清空"描述符，但我们可以使用一个占位符或者不更新
    // 实际上，在 bindless 渲染中，通常只需要确保不再使用该索引即可
    // 这里我们创建一个空的 ImageInfo，但更好的做法可能是使用一个默认的占位纹理
    // 为了简化，这里只移除映射，描述符集保持不变（shader 中会检查索引有效性）

    HK_LOG_INFO(ELogcat::Render, "纹理已从纹理池移除: Index={}", Index);
}

void FGlobalStaticResourcePool::AddSampler(const FRHISamplerDesc& SamplerDesc)
{
    // 根据 SamplerDesc 的 HashCode 查找是否已存在
    UInt64 HashCode = SamplerDesc.GetHashCode();
    if (SamplerIndexMap.Contains(HashCode))
    {
        return;
    }

    // 找到空槽位
    Int16 Index = FindEmptySamplerIndex();
    if (Index < 0)
    {
        HK_LOG_ERROR(ELogcat::Render, "采样器池已满，无法添加更多采样器");
        return;
    }

    // 创建采样器
    auto&       GfxDevice = GetGfxDeviceRef();
    FRHISampler Sampler   = GfxDevice.CreateSampler(SamplerDesc);
    if (!Sampler.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Render, "创建采样器失败");
        return;
    }

    // 将采样器添加到数组中
    SamplerArray[Index] = Sampler;
    SamplerIndexMap.Add(HashCode, Index);

    // 获取描述符集
    FRHIDescriptorSet StaticResourceDescriptorSet =
        FRHIPipelineResourcePool::GetRef().RequestCommonDescriptorSet(ECommonDescriptorSetIndex::StaticResource);

    // 更新描述符集（binding 1 是 Sampler）
    FRHIWriteDescriptorSet WriteDesc{};
    WriteDesc.DstBinding      = 1;
    WriteDesc.DstArrayElement = static_cast<UInt32>(Index);
    WriteDesc.DescriptorCount = 1;
    WriteDesc.DescriptorType  = ERHIDescriptorType::Sampler;

    FRHIDescriptorImageInfo ImageInfo{};
    ImageInfo.Sampler = Sampler;
    WriteDesc.ImageInfo.Add(ImageInfo);

    TArray<FRHIWriteDescriptorSet> WriteDescriptorSets;
    WriteDescriptorSets.Add(WriteDesc);
    GfxDevice.UpdateDescriptorSet(StaticResourceDescriptorSet, WriteDescriptorSets);

    HK_LOG_INFO(ELogcat::Render, "采样器已添加到采样器池: Index={}, HashCode={}", Index, HashCode);
}

Int16 FGlobalStaticResourcePool::GetTextureIndex(HTexture* InTexture) const
{
    if (InTexture == nullptr)
    {
        return -1;
    }

    const Int16* IndexPtr = TextureIndexMap.Find(InTexture);
    if (IndexPtr == nullptr)
    {
        return -1;
    }

    return *IndexPtr;
}

Int16 FGlobalStaticResourcePool::GetOrAddTextureIndex(HTexture* InTexture)
{
    if (InTexture == nullptr)
    {
        return -1;
    }

    // 先尝试获取索引
    Int16 Index = GetTextureIndex(InTexture);
    if (Index >= 0)
    {
        return Index;
    }

    // 不存在，则添加
    AddTexture(InTexture);

    // 再次获取索引（如果添加成功）
    return GetTextureIndex(InTexture);
}

Int16 FGlobalStaticResourcePool::GetSamplerIndex(const FRHISamplerDesc& SamplerDesc) const
{
    UInt64 HashCode = SamplerDesc.GetHashCode();
    const Int16* IndexPtr = SamplerIndexMap.Find(HashCode);
    if (IndexPtr == nullptr)
    {
        return -1;
    }

    return *IndexPtr;
}

Int16 FGlobalStaticResourcePool::GetOrAddSamplerIndex(const FRHISamplerDesc& SamplerDesc)
{
    // 先尝试获取索引
    Int16 Index = GetSamplerIndex(SamplerDesc);
    if (Index >= 0)
    {
        return Index;
    }

    // 不存在，则添加
    AddSampler(SamplerDesc);

    // 再次获取索引（如果添加成功）
    return GetSamplerIndex(SamplerDesc);
}

void FGlobalDynamicResourcePool::StartUp()
{
    ModelMatrixArray.Resize(HK_RENDER_INIT_MODEL_MATRIX_COUNT);
    FRHIBufferDesc BufferDesc{};

}

void FGlobalDynamicResourcePool::ShutDown()
{

}