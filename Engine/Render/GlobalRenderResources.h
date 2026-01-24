#pragma once
#include "Core/Container/Map.h"
#include "Core/Singleton/Singleton.h"
#include "Math/Matrix.h"
#include "RHI/RHIDescriptorSet.h"
#include "RHI/RHISampler.h"
#include "Render/RenderOptions.h"

class HTexture;
class FGlobalStaticResourcePool : public TSingleton<FGlobalStaticResourcePool>
{
    TMap<HTexture*, Int16>                                  TextureIndexMap;
    TFixedArray<HTexture*, HK_RENDER_BINDLESS_MAX_TEXTURES> TextureArray;

    // FRHISamplerDesc Hash to SamplerIndex
    TMap<UInt64, Int16>                                       SamplerIndexMap;
    TFixedArray<FRHISampler, HK_RENDER_BINDLESS_MAX_SAMPLERS> SamplerArray;

    Int16 FindEmptyTextureIndex();
    Int16 FindEmptySamplerIndex();

    // 移除纹理（由 Texture的PreDestroyEvent 调用）
    void RemoveTexture(HTexture* InTexture);
 
public:
    /**
     * 向纹理池中分配一个纹理, 如果纹理为空 or 纹理已经存在于纹理池中, 则不做任何操作
     * @param InTexture
     */
    void AddTexture(HTexture* InTexture);

    /**
     * 向采样器池中分配一个采样器, 如果采样器已经存在于采样器池中, 则不做任何操作
     * @param SamplerDesc
     */
    void AddSampler(const FRHISamplerDesc& SamplerDesc);

    /**
     * 获取纹理的索引
     * @param InTexture 纹理指针
     * @return 纹理索引，如果不存在返回 -1
     */
    Int16 GetTextureIndex(HTexture* InTexture) const;

    /**
     * 获取纹理的索引，如果不存在则添加并返回索引
     * @param InTexture 纹理指针
     * @return 纹理索引，如果添加失败返回 -1
     */
    Int16 GetOrAddTextureIndex(HTexture* InTexture);

    /**
     * 获取采样器的索引
     * @param SamplerDesc 采样器描述
     * @return 采样器索引，如果不存在返回 -1
     */
    Int16 GetSamplerIndex(const FRHISamplerDesc& SamplerDesc) const;

    /**
     * 获取采样器的索引，如果不存在则添加并返回索引
     * @param SamplerDesc 采样器描述
     * @return 采样器索引，如果添加失败返回 -1
     */
    Int16 GetOrAddSamplerIndex(const FRHISamplerDesc& SamplerDesc);
};

class FGlobalDynamicResourcePool : public TSingleton<FGlobalDynamicResourcePool>
{
    FRHIBuffer ModelMatrixBuffer;
    TArray<FMatrix4x4f> ModelMatrixArray;

public:
    void StartUp() override;
    void ShutDown() override;
};
