//
// Created by Admin on 2026/1/14.
//

#include "SharedMaterial.h"

#include "RHI/GfxDevice.h"
#include "Render/RenderOptions.h"
#include "Render/Shader/Shader.h"

FRHIDescriptorSetLayout FRHIPipelineResourcePool::RequestDescriptorSetLayout(
    const FRHIDescriptorSetLayoutDesc& DescriptorSetDesc)
{
    const UInt64 HashCode = DescriptorSetDesc.GetHashCode();

    if (auto* DescriptorSetLayout = DescriptorSetLayouts.Find(HashCode))
    {
        return DescriptorSetLayout->PipelineLayout;
    }

    FRHIDescriptorSetLayout NewDescriptorSetLayout = GetGfxDeviceRef().CreateDescriptorSetLayout(DescriptorSetDesc);
    FDescriptorSetLayoutRefCounter NewDescriptorSetLayoutRefCounter;
    NewDescriptorSetLayoutRefCounter.PipelineLayout = NewDescriptorSetLayout;
    NewDescriptorSetLayoutRefCounter.RefCount       = 1;
    DescriptorSetLayouts.Add(HashCode, NewDescriptorSetLayoutRefCounter);
    return NewDescriptorSetLayout;
}

void FRHIPipelineResourcePool::ReleaseDescriptorSetLayout(const UInt64& DescriptorSetLayoutHash)
{
    DescriptorSetLayouts.Remove(DescriptorSetLayoutHash);
}

FRHIPipelineLayout FRHIPipelineResourcePool::RequestPipelineLayout(const FRHIPipelineLayoutDesc& PipelineLayoutDesc)
{
    const UInt64 HashCode = PipelineLayoutDesc.GetHashCode();
    if (auto* PipelineLayout = PipelineLayouts.Find(HashCode))
    {
        return PipelineLayout->PipelineLayout;
    }
    FRHIPipelineLayout        NewPipelineLayout = GetGfxDeviceRef().CreatePipelineLayout(PipelineLayoutDesc);
    FPipelineLayoutRefCounter NewPipelineLayoutRefCounter;
    NewPipelineLayoutRefCounter.PipelineLayout = NewPipelineLayout;
    NewPipelineLayoutRefCounter.RefCount       = 1;
    PipelineLayouts.Add(HashCode, NewPipelineLayoutRefCounter);
    return NewPipelineLayout;
}

void FRHIPipelineResourcePool::ReleasePipelineLayout(const UInt64& PipelineLayoutHash)
{
    PipelineLayouts.Remove(PipelineLayoutHash);
}

FRHIDescriptorSet FRHIPipelineResourcePool::RequestCommonDescriptorSet(ECommonDescriptorSetIndex Index)
{
    if (CommonDescriptorSets[static_cast<Int32>(Index)].DescriptorSet.IsValid())
    {
        return CommonDescriptorSets[static_cast<Int32>(Index)].DescriptorSet;
    }
    const FRHIDescriptorSetLayout SetLayout = RequestCommonDescriptorSetLayout(Index);
    FRHIDescriptorSetDesc         DescriptorSetDesc;
    DescriptorSetDesc.Layout    = SetLayout;
    const char* DebugNameLUT[]  = {"DescSet_Camera", "DescSet_Model", "DescSet_StaticResource"};
    DescriptorSetDesc.DebugName = DebugNameLUT[static_cast<Int32>(Index)];

    CommonDescriptorSets[static_cast<Int32>(Index)].DescriptorSet =
        GetGfxDeviceRef().AllocateDescriptorSet(SelectDescriptorPool(Index), DescriptorSetDesc);
    return CommonDescriptorSets[static_cast<Int32>(Index)].DescriptorSet;
}

FRHIDescriptorSetLayout FRHIPipelineResourcePool::RequestCommonDescriptorSetLayout(ECommonDescriptorSetIndex Index)
{
    if (CommonDescriptorSets[static_cast<Int32>(Index)].Layout.IsValid())
    {
        return CommonDescriptorSets[static_cast<Int32>(Index)].Layout;
    }
    FRHIDescriptorSetLayoutDesc DescriptorSetDesc;
    auto&                       Device = GetGfxDeviceRef();
    switch (Index)
    {
        case ECommonDescriptorSetIndex::Camera:
        {
            DescriptorSetDesc.DebugName = "DescSetLayout_Camera";
            FRHIDescriptorSetLayoutBinding Binding{};
            Binding.Binding         = 0;
            Binding.DescriptorType  = ERHIDescriptorType::UniformBuffer;
            Binding.DescriptorCount = 1;
            Binding.StageFlags      = ERHIShaderStage::Vertex | ERHIShaderStage::Fragment;
            DescriptorSetDesc.Bindings.Add(Binding);
            CommonDescriptorSets[static_cast<Int32>(Index)].Layout = Device.CreateDescriptorSetLayout(DescriptorSetDesc);
        }
        break;
        case ECommonDescriptorSetIndex::Model:
        {
            DescriptorSetDesc.DebugName = "DescSetLayout_Model";
            FRHIDescriptorSetLayoutBinding Binding{};
            Binding.Binding         = 0;
            Binding.DescriptorType  = ERHIDescriptorType::StorageBuffer;
            Binding.DescriptorCount = 1;
            Binding.StageFlags      = ERHIShaderStage::Vertex;
            DescriptorSetDesc.Bindings.Add(Binding);
            CommonDescriptorSets[static_cast<Int32>(Index)].Layout = Device.CreateDescriptorSetLayout(DescriptorSetDesc);
        }
        break;
        case ECommonDescriptorSetIndex::StaticResource:
        {
            DescriptorSetDesc.DebugName = "DescSetLayout_StaticResource";
            FRHIDescriptorSetLayoutBinding Binding{};
            Binding.Binding         = 0;
            Binding.DescriptorType  = ERHIDescriptorType::SampledImage;
            Binding.DescriptorCount = HK_RENDER_BINDLESS_MAX_TEXTURES;
            Binding.StageFlags      = ERHIShaderStage::Vertex | ERHIShaderStage::Fragment;
            DescriptorSetDesc.Bindings.Add(Binding);
            FRHIDescriptorSetLayoutBinding Binding1{};
            Binding1.Binding         = 1;
            Binding1.DescriptorType  = ERHIDescriptorType::Sampler;
            Binding1.DescriptorCount = HK_RENDER_BINDLESS_MAX_SAMPLERS;
            Binding1.StageFlags      = ERHIShaderStage::Vertex | ERHIShaderStage::Fragment;
            DescriptorSetDesc.Bindings.Add(Binding1);
            CommonDescriptorSets[static_cast<Int32>(Index)].Layout = Device.CreateDescriptorSetLayout(DescriptorSetDesc);
        }
        default:
            HK_LOG_FATAL(ELogcat::Render, "Invalid CommonDescriptorSetIndex {}", static_cast<Int32>(Index));
            break;
    }
    return CommonDescriptorSets[static_cast<Int32>(Index)].Layout;
}

void FRHIPipelineResourcePool::StartUp()
{
    CreateGlobalDescriptorPools();
}

void FRHIPipelineResourcePool::ShutDown()
{
    ClearCommonDescriptorSets();
}

void FRHIPipelineResourcePool::ClearCommonDescriptorSets()
{
    for (Int32 Index = static_cast<Int32>(ECommonDescriptorSetIndex::Camera);
         Index < static_cast<Int32>(ECommonDescriptorSetIndex::Count); ++Index)
    {
        GetGfxDeviceRef().FreeDescriptorSet(StaticResourcesDescriptorPool, CommonDescriptorSets[Index].DescriptorSet);
        GetGfxDeviceRef().DestroyDescriptorSetLayout(CommonDescriptorSets[Index].Layout);
    }
    CommonDescriptorSets = {};
}

void FRHIPipelineResourcePool::CreateGlobalDescriptorPools()
{
    FRHIDescriptorPoolDesc DescriptorPoolDesc;
    DescriptorPoolDesc.DebugName = "StaticResourcesDescriptorPool";
    DescriptorPoolDesc.MaxSets   = 1;
    FRHIDescriptorPoolSize PoolSize{};
    PoolSize.Type  = ERHIDescriptorType::SampledImage;
    PoolSize.Count = HK_RENDER_BINDLESS_MAX_TEXTURES;
    DescriptorPoolDesc.PoolSizes.Add(PoolSize);
    PoolSize.Type  = ERHIDescriptorType::Sampler;
    PoolSize.Count = HK_RENDER_BINDLESS_MAX_SAMPLERS;
    DescriptorPoolDesc.PoolSizes.Add(PoolSize);
    DescriptorPoolDesc.Flags =
        ERHIDescriptorPoolCreateFlag::UpdateAfterBind | ERHIDescriptorPoolCreateFlag::FreeDescriptorSet;
    StaticResourcesDescriptorPool = GetGfxDeviceRef().CreateDescriptorPool(DescriptorPoolDesc);

    // Dynamic
    FRHIDescriptorPoolDesc DynamicDescriptorPoolDesc;
    DynamicDescriptorPoolDesc.DebugName = "DynamicDescriptorPool";
    DynamicDescriptorPoolDesc.MaxSets   = 2;
    FRHIDescriptorPoolSize DynamicPoolSize{};
    DynamicPoolSize.Type  = ERHIDescriptorType::UniformBuffer;
    DynamicPoolSize.Count = HK_RENDER_BINDLESS_MAX_UNIFORM_BUFFERS;
    DynamicDescriptorPoolDesc.PoolSizes.Add(DynamicPoolSize);
    DynamicPoolSize.Type  = ERHIDescriptorType::StorageBuffer;
    DynamicPoolSize.Count = HK_RENDER_BINDLESS_MAX_STORAGE_BUFFERS;
    DynamicDescriptorPoolDesc.PoolSizes.Add(DynamicPoolSize);
    DynamicDescriptorPoolDesc.Flags =
        ERHIDescriptorPoolCreateFlag::UpdateAfterBind | ERHIDescriptorPoolCreateFlag::FreeDescriptorSet;
    DynamicResourcesDescriptorPool = GetGfxDeviceRef().CreateDescriptorPool(DynamicDescriptorPoolDesc);
}

void FRHIPipelineResourcePool::DestroyGlobalDescriptorPools()
{
    GetGfxDeviceRef().DestroyDescriptorPool(StaticResourcesDescriptorPool);
    GetGfxDeviceRef().DestroyDescriptorPool(DynamicResourcesDescriptorPool);
}

FRHIDescriptorPool FRHIPipelineResourcePool::SelectDescriptorPool(ECommonDescriptorSetIndex Index)
{
    switch (Index)
    {
        case ECommonDescriptorSetIndex::Camera:
        case ECommonDescriptorSetIndex::Model:
            return DynamicResourcesDescriptorPool;
        case ECommonDescriptorSetIndex::StaticResource:
            return StaticResourcesDescriptorPool;
        case ECommonDescriptorSetIndex::Count:
            break;
    }
    return {};
}

FSharedMaterial::FSharedMaterial(const HShader* InShader)
{
    if (InShader == nullptr)
    {
        return;
    }
    auto& TranslateResult = InShader->GetCompileResult();
    if (!TranslateResult.IsValid())
    {
        return;
    }
    const auto& ParameterSheet = TranslateResult.ParameterSheet;
    auto&       ResourcePool   = FRHIPipelineResourcePool::GetRef();
    auto&       GfxDevice      = GetGfxDeviceRef();

    // 1. 使用 HShader::Compile 编译着色器模块
    TFixedArray<FRHIShaderModule, 2> ShaderModules;
    // 需要非 const 指针来调用 Compile，但这里只是读取编译结果，所以使用 const_cast
    // 或者更好的方式是修改函数签名，但为了保持兼容性，这里使用 const_cast
    HShader* NonConstShader = const_cast<HShader*>(InShader);
    if (!NonConstShader->Compile(ShaderModules, false)) // ClearCode = false，保留代码
    {
        return;
    }
    FRHIShaderModule VSModule = ShaderModules[0];
    FRHIShaderModule FSModule = ShaderModules[1];

    HK_DEFER(
        [&VSModule, &FSModule]
        {
            GetGfxDeviceRef().DestroyShaderModule(VSModule);
            GetGfxDeviceRef().DestroyShaderModule(FSModule);
        });

    // 2. 根据 ParameterSheet 配置 DescriptorSetLayout
    FRHIPipelineLayoutDesc PipelineLayoutDesc;
    PipelineLayoutDesc.DebugName = std::format("PipelineLayout_{}", InShader->GetName());

    // 根据需求添加公共 DescriptorSet
    if (ParameterSheet.bNeedCamera)
    {
        FRHIDescriptorSetLayout CameraLayout =
            ResourcePool.RequestCommonDescriptorSetLayout(ECommonDescriptorSetIndex::Camera);
        PipelineLayoutDesc.SetLayouts.Add(CameraLayout);
    }
    if (ParameterSheet.bNeedModel)
    {
        FRHIDescriptorSetLayout ModelLayout =
            ResourcePool.RequestCommonDescriptorSetLayout(ECommonDescriptorSetIndex::Model);
        PipelineLayoutDesc.SetLayouts.Add(ModelLayout);
    }
    if (ParameterSheet.bNeedResourcePool)
    {
        FRHIDescriptorSetLayout StaticResourceLayout =
            ResourcePool.RequestCommonDescriptorSetLayout(ECommonDescriptorSetIndex::StaticResource);
        PipelineLayoutDesc.SetLayouts.Add(StaticResourceLayout);
    }

    // 3. 配置 PushConstant
    // 根据 SimpleShading.slang，InstancePushConstants 包含 3 个 uint（ModelID, MainTextureID, MainSamplerStateID）
    // 总共 12 字节，需要在 Vertex 和 Fragment 阶段都可用
    if (!ParameterSheet.PushConstants.IsEmpty())
    {
        // 计算 PushConstant 的总大小（找到最小 offset 和最大 end offset）
        UInt32 MinOffset    = ParameterSheet.PushConstants[0].Offset;
        UInt32 MaxEndOffset = ParameterSheet.PushConstants[0].Offset + ParameterSheet.PushConstants[0].Size;
        for (size_t i = 1; i < ParameterSheet.PushConstants.Size(); ++i)
        {
            const auto& PushConstant = ParameterSheet.PushConstants[i];
            if (PushConstant.Offset < MinOffset)
            {
                MinOffset = PushConstant.Offset;
            }
            UInt32 EndOffset = PushConstant.Offset + PushConstant.Size;
            if (EndOffset > MaxEndOffset)
            {
                MaxEndOffset = EndOffset;
            }
        }
        // 计算总大小并确保对齐到 4 字节边界
        UInt32 TotalSize = MaxEndOffset - MinOffset;
        TotalSize        = (TotalSize + 3) & ~3; // 对齐到 4 字节边界

        // PushConstantRanges 格式：每4个UInt32表示一个范围（offset, size, stageFlags, 保留）
        // 根据 SimpleShading.slang，PushConstant 在 Vertex 和 Fragment 阶段都使用
        PipelineLayoutDesc.PushConstantRanges.Add(MinOffset); // offset
        PipelineLayoutDesc.PushConstantRanges.Add(TotalSize); // size
        PipelineLayoutDesc.PushConstantRanges.Add(
            static_cast<UInt32>(ERHIShaderStage::Vertex | ERHIShaderStage::Fragment)); // stageFlags
        PipelineLayoutDesc.PushConstantRanges.Add(0);                                  // 保留字段
    }

    // 4. 创建 PipelineLayout
    FRHIPipelineLayout PipelineLayout = ResourcePool.RequestPipelineLayout(PipelineLayoutDesc);
    PipelineLayoutHash                = PipelineLayoutDesc.GetHashCode();

    // 5. 配置 PipelineDesc
    FRHIGraphicsPipelineDesc PipelineDesc;
    PipelineDesc.Layout     = PipelineLayout;
    PipelineDesc.DebugName  = std::format("GraphicsPipeline_{}", InShader->GetName());
    PipelineDesc.RenderPass = nullptr; // 使用 dynamic rendering
    PipelineDesc.Subpass    = 0;

    // 5.1 配置着色器阶段
    PipelineDesc.ShaderStageState.ShaderModules.Add(VSModule);
    PipelineDesc.ShaderStageState.ShaderModules.Add(FSModule);

    // 5.2 配置顶点输入（Vertex_PNU 格式）
    // Position: float3 (location 0, 12 字节, offset 0)
    // Normal: float3 (location 1, 12 字节, offset 12)
    // UV: float2 (location 2, 8 字节, offset 24)
    FRHIVertexInputBindingDescription VertexBinding;
    VertexBinding.Binding       = 0;
    VertexBinding.Stride        = 32; // 3 * 4 + 3 * 4 + 2 * 4 = 32 字节
    VertexBinding.bInstanceRate = false;
    PipelineDesc.VertexInputState.VertexBindings.Add(VertexBinding);

    FRHIVertexInputAttributeDescription PositionAttr;
    PositionAttr.Location = 0;
    PositionAttr.Binding  = 0;
    PositionAttr.Format   = ERHIImageFormat::R32G32B32_SFloat; // float3
    PositionAttr.Offset   = 0;
    PipelineDesc.VertexInputState.VertexAttributes.Add(PositionAttr);

    FRHIVertexInputAttributeDescription NormalAttr;
    NormalAttr.Location = 1;
    NormalAttr.Binding  = 0;
    NormalAttr.Format   = ERHIImageFormat::R32G32B32_SFloat; // float3
    NormalAttr.Offset   = 12;                                // Position 占用 12 字节
    PipelineDesc.VertexInputState.VertexAttributes.Add(NormalAttr);

    FRHIVertexInputAttributeDescription UVAttr;
    UVAttr.Location = 2;
    UVAttr.Binding  = 0;
    UVAttr.Format   = ERHIImageFormat::R32G32_SFloat; // float2
    UVAttr.Offset   = 24;                             // Position + Normal 占用 24 字节
    PipelineDesc.VertexInputState.VertexAttributes.Add(UVAttr);

    // 5.3 配置输入装配（使用默认值：TriangleList）
    PipelineDesc.InputAssemblyState.PrimitiveTopology       = ERHIPrimitiveTopology::TriangleList;
    PipelineDesc.InputAssemblyState.bPrimitiveRestartEnable = false;

    // 5.4 配置视口状态（使用动态视口）
    PipelineDesc.ViewportState.Viewports.Clear();
    PipelineDesc.ViewportState.Scissors.Clear();

    // 5.5 配置光栅化状态（使用默认值）
    PipelineDesc.RasterizationState.bDepthClampEnable        = false;
    PipelineDesc.RasterizationState.bRasterizerDiscardEnable = false;
    PipelineDesc.RasterizationState.PolygonMode              = ERHIPolygonMode::Fill;
    PipelineDesc.RasterizationState.CullMode                 = ERHICullMode::Back;
    PipelineDesc.RasterizationState.FrontFace                = ERHIFrontFace::CounterClockwise;
    PipelineDesc.RasterizationState.bDepthBiasEnable         = false;
    PipelineDesc.RasterizationState.DepthBiasConstantFactor  = 0.0f;
    PipelineDesc.RasterizationState.DepthBiasClamp           = 0.0f;
    PipelineDesc.RasterizationState.DepthBiasSlopeFactor     = 0.0f;
    PipelineDesc.RasterizationState.LineWidth                = 1.0f;

    // 5.6 配置多重采样状态（使用默认值）
    PipelineDesc.MultisampleState.RasterizationSamples   = ERHISampleCount::Sample1;
    PipelineDesc.MultisampleState.bSampleShadingEnable   = false;
    PipelineDesc.MultisampleState.MinSampleShading       = 0.0f;
    PipelineDesc.MultisampleState.SampleMask             = 0xFFFFFFFF;
    PipelineDesc.MultisampleState.bAlphaToCoverageEnable = false;
    PipelineDesc.MultisampleState.bAlphaToOneEnable      = false;

    // 5.7 配置深度模板状态（使用默认值）
    PipelineDesc.DepthStencilState.bDepthTestEnable       = true;
    PipelineDesc.DepthStencilState.bDepthWriteEnable      = true;
    PipelineDesc.DepthStencilState.DepthCompareOp         = ERHICompareOp::Less;
    PipelineDesc.DepthStencilState.bDepthBoundsTestEnable = false;
    PipelineDesc.DepthStencilState.bStencilTestEnable     = false;

    // 5.8 配置颜色混合状态（使用默认值：一个颜色附件，不启用混合）
    FRHIBlendAttachmentState BlendAttachment;
    BlendAttachment.bBlendEnable        = false;
    BlendAttachment.SrcColorBlendFactor = ERHIBlendFactor::One;
    BlendAttachment.DstColorBlendFactor = ERHIBlendFactor::Zero;
    BlendAttachment.ColorBlendOp        = ERHIBlendOp::Add;
    BlendAttachment.SrcAlphaBlendFactor = ERHIBlendFactor::One;
    BlendAttachment.DstAlphaBlendFactor = ERHIBlendFactor::Zero;
    BlendAttachment.AlphaBlendOp        = ERHIBlendOp::Add;
    BlendAttachment.ColorWriteMask =
        ERHIColorComponentFlag::R | ERHIColorComponentFlag::G | ERHIColorComponentFlag::B | ERHIColorComponentFlag::A;
    PipelineDesc.ColorBlendState.BlendAttachments.Add(BlendAttachment);
    PipelineDesc.ColorBlendState.bLogicOpEnable = false;
    PipelineDesc.ColorBlendState.LogicOp        = ERHILogicOp::Copy;

    // 5.9 配置动态状态（视口和裁剪矩形使用动态）
    PipelineDesc.DynamicState.DynamicStates = ERHIDynamicState::Viewport | ERHIDynamicState::Scissor;

    // 6. 创建 Pipeline
    Pipeline = GfxDevice.CreateGraphicsPipeline(PipelineDesc);
}

bool FSharedMaterial::IsValid() const
{
    return Pipeline.IsValid() && PipelineLayoutHash != 0;
}

TSharedPtr<FSharedMaterial> FSharedMaterialManager::RequestSharedMaterial(const HShader* InShader)
{
    if (InShader == nullptr)
    {
        return nullptr;
    }
    const auto HashCode = InShader->GetHashCode();
    if (auto* SharedMaterial = SharedMaterials.Find(HashCode))
    {
        if (!SharedMaterial->IsExpired())
        {
            return SharedMaterial->Lock();
        }
    }
    TSharedPtr<FSharedMaterial> NewSharedMaterial = MakeShared<FSharedMaterial>(InShader);
    SharedMaterials.Add(HashCode, NewSharedMaterial);
    return NewSharedMaterial;
}
