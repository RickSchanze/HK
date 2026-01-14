#pragma once
#include "Core/Utility/SharedPtr.h"
#include "RHI/RHIPipeline.h"

struct FRHIPipelineResourcePool
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

    TMap<UInt64, FPipelineLayoutRefCounter>      Pipelines;
    TMap<UInt64, FDescriptorSetLayoutRefCounter> DescriptorSetLayouts;

    FRHIDescriptorSetLayout RequestDescriptorSetLayout(const FRHIDescriptorSetLayoutDesc& DescriptorSetDesc);
    void                    ReleaseDescriptorSetLayout(const UInt64& DescriptorSetLayoutHash);

    FRHIPipelineLayout RequestPipelineLayout(const FRHIPipelineLayoutDesc& PipelineLayoutDesc);
    void               ReleasePipelineLayout(const UInt64& PipelineLayoutHash);
};

struct FSharedMaterial
{
    FRHIPipeline Pipeline;
    UInt64       PipelineLayoutHash;
};
