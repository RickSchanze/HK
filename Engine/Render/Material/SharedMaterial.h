#pragma once
#include "Core/Utility/SharedPtr.h"
#include "Core/Utility/WeakPtr.h"
#include "RHI/RHIPipeline.h"

class HShader;

enum class ECommonDescriptorSetIndex
{
    Camera,         // 摄像机矩阵
    Model,          // 模型矩阵
    StaticResource, // 纹理, 采样器等
    Count,
};
HK_ENUM_INCREMENT(ECommonDescriptorSetIndex)

struct FRHIPipelineResourcePool : TSingleton<FRHIPipelineResourcePool>
{
    struct FPipelineLayoutRefCounter
    {
        FRHIPipelineLayout PipelineLayout;
        Int32              RefCount;
    };

    struct FDescriptorSetLayoutRefCounter
    {
        FRHIDescriptorSetLayout PipelineLayout;
        Int32                   RefCount;
    };

    TMap<UInt64, FPipelineLayoutRefCounter>      PipelineLayouts;
    TMap<UInt64, FDescriptorSetLayoutRefCounter> DescriptorSetLayouts;

    // 有极大概率会用到的DescriptorSet
    struct FFixedDescriptorSet
    {
        FRHIDescriptorSetLayout Layout;
        FRHIDescriptorSet       DescriptorSet;
    };
    TFixedArray<FFixedDescriptorSet, static_cast<Int32>(ECommonDescriptorSetIndex::Count)> CommonDescriptorSets;

    // 用于静态资产(纹理, 采样器等)的DescriptorPool, 开启了UpdateAfterBind
    FRHIDescriptorPool StaticResourcesDescriptorPool;
    // 每帧都可能变化的DescriptorPool, 一大用处是模型矩阵
    FRHIDescriptorPool DynamicResourcesDescriptorPool;

    FRHIDescriptorSetLayout RequestDescriptorSetLayout(const FRHIDescriptorSetLayoutDesc& DescriptorSetDesc);
    void                    ReleaseDescriptorSetLayout(const UInt64& DescriptorSetLayoutHash);

    FRHIPipelineLayout RequestPipelineLayout(const FRHIPipelineLayoutDesc& PipelineLayoutDesc);
    void               ReleasePipelineLayout(const UInt64& PipelineLayoutHash);

    FRHIDescriptorSet       RequestCommonDescriptorSet(ECommonDescriptorSetIndex Index);
    FRHIDescriptorSetLayout RequestCommonDescriptorSetLayout(ECommonDescriptorSetIndex Index);

    void StartUp() override;
    void ShutDown() override;

    void ClearCommonDescriptorSets();

    void CreateGlobalDescriptorPools();
    void DestroyGlobalDescriptorPools();

    FRHIDescriptorPool SelectDescriptorPool(ECommonDescriptorSetIndex Index);
};

struct FSharedMaterial
{
    FRHIPipeline Pipeline;
    UInt64       PipelineLayoutHash;

    explicit FSharedMaterial(const HShader* InShader);

    bool IsValid() const;
};

struct FSharedMaterialManager : TSingleton<FSharedMaterialManager>
{
    TMap<UInt64, TWeakPtr<FSharedMaterial>> SharedMaterials;

    // 存的是弱引用, 不会影响SharedPtr正常引用技术
    // SharedPtr会自己管理生命周期, 不需要认为去Release
    TSharedPtr<FSharedMaterial> RequestSharedMaterial(const HShader* InShader);
};
