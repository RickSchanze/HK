//
// Created by Admin on 2025/12/21.
//

#include "GfxDeviceVk.h"

#include "Core/Container/Array.h"
#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "RHI/RHIDescriptorSet.h"
#include "RHI/RHIHandle.h"

#pragma region ShaderModule实现

FRHIShaderModule FGfxDeviceVk::CreateShaderModule(const FRHIShaderModuleDesc& ModuleCreateInfo, ERHIShaderStage Stage)
{
    try
    {
        if (ModuleCreateInfo.Code.IsEmpty())
        {
            HK_LOG_FATAL(ELogcat::RHI, "着色器模块代码为空");
            throw std::runtime_error("着色器模块代码为空");
        }

        // 创建 Vulkan 着色器模块创建信息
        vk::ShaderModuleCreateInfo ModuleInfo;
        ModuleInfo.codeSize = ModuleCreateInfo.Code.Size() * sizeof(UInt32);
        ModuleInfo.pCode = ModuleCreateInfo.Code.Data();

        // 创建 Vulkan 着色器模块
        vk::ShaderModule VulkanModule;
        try
        {
            VulkanModule = Device.createShaderModule(ModuleInfo);
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_FATAL(ELogcat::RHI, "创建 Vulkan 着色器模块失败: {}", e.what());
            throw std::runtime_error((FString("创建 Vulkan 着色器模块失败: ") + FString(e.what())).CStr());
        }

        // 设置调试名称
        if (!ModuleCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanModule, vk::ObjectType::eShaderModule, ModuleCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto& HandleManager = FRHIHandleManager::GetRef();
        const FRHIHandle ModuleHandle = HandleManager.CreateRHIHandle(
            ModuleCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkShaderModule>(VulkanModule)));

        // 创建 FRHIShaderModule 对象
        FRHIShaderModule ShaderModule;
        ShaderModule.Handle = ModuleHandle;
        ShaderModule.Stage = Stage;

        HK_LOG_INFO(ELogcat::RHI, "着色器模块创建成功: Stage={}, CodeSize={}", static_cast<UInt32>(Stage),
                    ModuleCreateInfo.Code.Size() * sizeof(UInt32));

        return ShaderModule;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建着色器模块失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建着色器模块失败: 未知异常");
        throw std::runtime_error("创建着色器模块失败: 未知异常");
    }
}

void FGfxDeviceVk::DestroyShaderModule(FRHIShaderModule& ShaderModule)
{
    if (!ShaderModule.IsValid())
    {
        return;
    }

    // 获取 Vulkan 着色器模块句柄
    const auto VulkanModule = vk::ShaderModule(ShaderModule.Handle.Cast<VkShaderModule>());

    // 销毁 Vulkan 着色器模块
    if (Device && VulkanModule)
    {
        Device.destroyShaderModule(VulkanModule);
    }

    // 销毁 RHI 句柄
    auto& HandleManager = FRHIHandleManager::GetRef();
    HandleManager.DestroyRHIHandle(ShaderModule.Handle);

    // 重置着色器模块对象
    ShaderModule = FRHIShaderModule();

    HK_LOG_INFO(ELogcat::RHI, "着色器模块已销毁");
}

#pragma endregion

#pragma region PipelineLayout实现

FRHIPipelineLayout FGfxDeviceVk::CreatePipelineLayout(const FRHIPipelineLayoutDesc& LayoutCreateInfo)
{
    try
    {
        // 转换描述符集布局数组
        TArray<vk::DescriptorSetLayout> VulkanSetLayouts;
        VulkanSetLayouts.Reserve(LayoutCreateInfo.SetLayouts.Size());

        for (const FRHIDescriptorSetLayout& SetLayout : LayoutCreateInfo.SetLayouts)
        {
            if (SetLayout.IsValid())
            {
                VulkanSetLayouts.Add(vk::DescriptorSetLayout(SetLayout.Handle.Cast<VkDescriptorSetLayout>()));
            }
        }

        // 转换推送常量范围
        TArray<vk::PushConstantRange> VulkanPushConstantRanges;
        if (!LayoutCreateInfo.PushConstantRanges.IsEmpty())
        {
            // 每4个UInt32表示一个范围：offset, size, stageFlags, 保留
            const UInt32 RangeCount = LayoutCreateInfo.PushConstantRanges.Size() / 4;
            VulkanPushConstantRanges.Reserve(RangeCount);

            for (UInt32 i = 0; i < RangeCount; ++i)
            {
                const UInt32 BaseIdx = i * 4;
                vk::PushConstantRange Range;
                Range.offset = LayoutCreateInfo.PushConstantRanges[BaseIdx];
                Range.size = LayoutCreateInfo.PushConstantRanges[BaseIdx + 1];
                Range.stageFlags = ConvertShaderStageFlags(
                    static_cast<ERHIShaderStage>(LayoutCreateInfo.PushConstantRanges[BaseIdx + 2]));
                VulkanPushConstantRanges.Add(Range);
            }
        }

        // 创建 Vulkan 管线布局创建信息
        vk::PipelineLayoutCreateInfo LayoutInfo;
        LayoutInfo.setLayoutCount = static_cast<uint32_t>(VulkanSetLayouts.Size());
        LayoutInfo.pSetLayouts = VulkanSetLayouts.Data();
        LayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(VulkanPushConstantRanges.Size());
        LayoutInfo.pPushConstantRanges = VulkanPushConstantRanges.Data();

        // 创建 Vulkan 管线布局
        vk::PipelineLayout VulkanLayout;
        try
        {
            VulkanLayout = Device.createPipelineLayout(LayoutInfo);
        }
        catch (const vk::SystemError& e)
        {
            HK_LOG_FATAL(ELogcat::RHI, "创建 Vulkan 管线布局失败: {}", e.what());
            throw std::runtime_error((FString("创建 Vulkan 管线布局失败: ") + FString(e.what())).CStr());
        }

        // 设置调试名称
        if (!LayoutCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanLayout, vk::ObjectType::ePipelineLayout, LayoutCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto& HandleManager = FRHIHandleManager::GetRef();
        const FRHIHandle LayoutHandle = HandleManager.CreateRHIHandle(
            LayoutCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkPipelineLayout>(VulkanLayout)));

        // 创建 FRHIPipelineLayout 对象
        FRHIPipelineLayout PipelineLayout;
        PipelineLayout.Handle = LayoutHandle;

        HK_LOG_INFO(ELogcat::RHI, "管线布局创建成功: SetLayouts={}, PushConstantRanges={}", VulkanSetLayouts.Size(),
                    VulkanPushConstantRanges.Size());

        return PipelineLayout;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建管线布局失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建管线布局失败: 未知异常");
        throw std::runtime_error("创建管线布局失败: 未知异常");
    }
}

void FGfxDeviceVk::DestroyPipelineLayout(FRHIPipelineLayout& PipelineLayout)
{
    if (!PipelineLayout.IsValid())
    {
        return;
    }

    // 获取 Vulkan 管线布局句柄
    const auto VulkanLayout = vk::PipelineLayout(PipelineLayout.Handle.Cast<VkPipelineLayout>());

    // 销毁 Vulkan 管线布局
    if (Device && VulkanLayout)
    {
        Device.destroyPipelineLayout(VulkanLayout);
    }

    // 销毁 RHI 句柄
    auto& HandleManager = FRHIHandleManager::GetRef();
    HandleManager.DestroyRHIHandle(PipelineLayout.Handle);

    // 重置管线布局对象
    PipelineLayout = FRHIPipelineLayout();

    HK_LOG_INFO(ELogcat::RHI, "管线布局已销毁");
}

#pragma endregion

#pragma region Pipeline实现

FRHIPipeline FGfxDeviceVk::CreateGraphicsPipeline(const FRHIGraphicsPipelineDesc& PipelineCreateInfo)
{
    HK_ASSERT_MSG_RAW(PipelineCreateInfo.Layout.IsValid(), "无效的管线布局");

    try
    {
        // 获取 Vulkan 管线布局
        const auto VulkanLayout = vk::PipelineLayout(PipelineCreateInfo.Layout.Handle.Cast<VkPipelineLayout>());

        const vk::Pipeline VulkanPipeline = CreateGraphicsPipelineInternal(VulkanLayout, PipelineCreateInfo);

        if (!VulkanPipeline)
        {
            HK_LOG_FATAL(ELogcat::RHI, "图形管线创建失败");
            throw std::runtime_error("图形管线创建失败");
        }

        // 设置调试名称
        if (!PipelineCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanPipeline, vk::ObjectType::ePipeline, PipelineCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto& HandleManager = FRHIHandleManager::GetRef();
        const FRHIHandle PipelineHandle = HandleManager.CreateRHIHandle(
            PipelineCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkPipeline>(VulkanPipeline)));

        // 创建 FRHIPipeline 对象
        FRHIPipeline Pipeline;
        Pipeline.Handle = PipelineHandle;
        Pipeline.Type = ERHIPipelineType::Graphics;
        Pipeline.Layout = PipelineCreateInfo.Layout; // 值类型拷贝

        HK_LOG_INFO(ELogcat::RHI, "图形管线创建成功");

        return Pipeline;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建图形管线失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建图形管线失败: 未知异常");
        throw std::runtime_error("创建图形管线失败: 未知异常");
    }
}

FRHIPipeline FGfxDeviceVk::CreateComputePipeline(const FRHIComputePipelineDesc& PipelineCreateInfo)
{
    HK_ASSERT_MSG_RAW(PipelineCreateInfo.Layout.IsValid(), "无效的管线布局");
    HK_ASSERT_MSG_RAW(PipelineCreateInfo.ComputeShader.IsValid(), "无效的计算着色器");

    try
    {
        // 获取 Vulkan 管线布局
        const auto VulkanLayout = vk::PipelineLayout(PipelineCreateInfo.Layout.Handle.Cast<VkPipelineLayout>());

        const vk::Pipeline VulkanPipeline = CreateComputePipelineInternal(VulkanLayout, PipelineCreateInfo);

        if (!VulkanPipeline)
        {
            HK_LOG_FATAL(ELogcat::RHI, "计算管线创建失败");
            throw std::runtime_error("计算管线创建失败");
        }

        // 设置调试名称
        if (!PipelineCreateInfo.DebugName.IsEmpty())
        {
            SetDebugName(VulkanPipeline, vk::ObjectType::ePipeline, PipelineCreateInfo.DebugName);
        }

        // 创建 RHI 句柄
        auto& HandleManager = FRHIHandleManager::GetRef();
        const FRHIHandle PipelineHandle = HandleManager.CreateRHIHandle(
            PipelineCreateInfo.DebugName.CStr(), reinterpret_cast<void*>(static_cast<VkPipeline>(VulkanPipeline)));

        // 创建 FRHIPipeline 对象
        FRHIPipeline Pipeline;
        Pipeline.Handle = PipelineHandle;
        Pipeline.Type = ERHIPipelineType::Compute;
        Pipeline.Layout = PipelineCreateInfo.Layout; // 值类型拷贝

        HK_LOG_INFO(ELogcat::RHI, "计算管线创建成功");

        return Pipeline;
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建计算管线失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建计算管线失败: 未知异常");
        throw std::runtime_error("创建计算管线失败: 未知异常");
    }
}

FRHIPipeline FGfxDeviceVk::CreateRayTracingPipeline(const FRHIRayTracingPipelineDesc& PipelineCreateInfo)
{
    HK_ASSERT_MSG_RAW(PipelineCreateInfo.Layout.IsValid(), "无效的管线布局");

    try
    {
        HK_LOG_WARN(ELogcat::RHI, "光线追踪管线创建暂未实现");
        throw std::runtime_error("光线追踪管线创建暂未实现");
    }
    catch (const std::exception& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建光线追踪管线失败: {}", e.what());
        throw;
    }
    catch (...)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建光线追踪管线失败: 未知异常");
        throw std::runtime_error("创建光线追踪管线失败: 未知异常");
    }
}

// 内部辅助函数：创建图形管线
vk::Pipeline FGfxDeviceVk::CreateGraphicsPipelineInternal(vk::PipelineLayout Layout,
                                                          const FRHIGraphicsPipelineDesc& Desc) const
{
    // 1. Shader stages
    TArray<vk::PipelineShaderStageCreateInfo> ShaderStages;
    ShaderStages.Reserve(Desc.ShaderStageState.ShaderModules.Size());

    for (const FRHIShaderModule& ShaderModule : Desc.ShaderStageState.ShaderModules)
    {
        if (ShaderModule.IsValid())
        {
            vk::PipelineShaderStageCreateInfo StageInfo;
            StageInfo.stage = ConvertShaderStage(ShaderModule.GetStage());
            StageInfo.module = vk::ShaderModule(ShaderModule.GetHandle().Cast<VkShaderModule>());
            StageInfo.pName = "main"; // 默认入口点
            ShaderStages.Add(StageInfo);
        }
    }

    if (ShaderStages.IsEmpty())
    {
        HK_LOG_FATAL(ELogcat::RHI, "图形管线需要至少一个着色器模块");
        throw std::runtime_error("图形管线需要至少一个着色器模块");
    }

    // 2. Vertex input
    TArray<vk::VertexInputBindingDescription> VertexBindings;
    VertexBindings.Reserve(Desc.VertexInputState.VertexBindings.Size());
    for (const auto& Binding : Desc.VertexInputState.VertexBindings)
    {
        vk::VertexInputBindingDescription VkBinding;
        VkBinding.binding = Binding.Binding;
        VkBinding.stride = Binding.Stride;
        VkBinding.inputRate = Binding.bInstanceRate ? vk::VertexInputRate::eInstance : vk::VertexInputRate::eVertex;
        VertexBindings.Add(VkBinding);
    }

    TArray<vk::VertexInputAttributeDescription> VertexAttributes;
    VertexAttributes.Reserve(Desc.VertexInputState.VertexAttributes.Size());
    for (const auto& Attr : Desc.VertexInputState.VertexAttributes)
    {
        vk::VertexInputAttributeDescription VkAttr;
        VkAttr.location = Attr.Location;
        VkAttr.binding = Attr.Binding;
        VkAttr.format = ConvertImageFormat(Attr.Format);
        VkAttr.offset = Attr.Offset;
        VertexAttributes.Add(VkAttr);
    }

    vk::PipelineVertexInputStateCreateInfo VertexInputInfo;
    VertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(VertexBindings.Size());
    VertexInputInfo.pVertexBindingDescriptions = VertexBindings.Data();
    VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttributes.Size());
    VertexInputInfo.pVertexAttributeDescriptions = VertexAttributes.Data();

    // 3. Input assembly
    vk::PipelineInputAssemblyStateCreateInfo InputAssembly;
    InputAssembly.topology = ConvertPrimitiveTopology(Desc.InputAssemblyState.PrimitiveTopology);
    InputAssembly.primitiveRestartEnable = Desc.InputAssemblyState.bPrimitiveRestartEnable ? VK_TRUE : VK_FALSE;

    // 4. Viewport and scissor
    TArray<vk::Viewport> Viewports;
    Viewports.Reserve(Desc.ViewportState.Viewports.Size());
    for (const auto& Vp : Desc.ViewportState.Viewports)
    {
        vk::Viewport VkViewport;
        VkViewport.x = Vp.X;
        VkViewport.y = Vp.Y;
        VkViewport.width = Vp.Width;
        VkViewport.height = Vp.Height;
        VkViewport.minDepth = Vp.MinDepth;
        VkViewport.maxDepth = Vp.MaxDepth;
        Viewports.Add(VkViewport);
    }

    TArray<vk::Rect2D> Scissors;
    Scissors.Reserve(Desc.ViewportState.Scissors.Size());
    for (const auto& Sc : Desc.ViewportState.Scissors)
    {
        vk::Rect2D VkScissor;
        VkScissor.offset = vk::Offset2D(Sc.X, Sc.Y);
        VkScissor.extent = vk::Extent2D(Sc.Width, Sc.Height);
        Scissors.Add(VkScissor);
    }

    vk::PipelineViewportStateCreateInfo ViewportState;
    ViewportState.viewportCount = static_cast<uint32_t>(Viewports.Size());
    ViewportState.pViewports = Viewports.Data();
    ViewportState.scissorCount = static_cast<uint32_t>(Scissors.Size());
    ViewportState.pScissors = Scissors.Data();

    // 5. Rasterization
    vk::PipelineRasterizationStateCreateInfo Rasterizer;
    Rasterizer.depthClampEnable = Desc.RasterizationState.bDepthClampEnable ? VK_TRUE : VK_FALSE;
    Rasterizer.rasterizerDiscardEnable = Desc.RasterizationState.bRasterizerDiscardEnable ? VK_TRUE : VK_FALSE;
    Rasterizer.polygonMode = ConvertPolygonMode(Desc.RasterizationState.PolygonMode);
    Rasterizer.cullMode = ConvertCullMode(Desc.RasterizationState.CullMode);
    Rasterizer.frontFace = ConvertFrontFace(Desc.RasterizationState.FrontFace);
    Rasterizer.depthBiasEnable = Desc.RasterizationState.bDepthBiasEnable ? VK_TRUE : VK_FALSE;
    Rasterizer.depthBiasConstantFactor = Desc.RasterizationState.DepthBiasConstantFactor;
    Rasterizer.depthBiasClamp = Desc.RasterizationState.DepthBiasClamp;
    Rasterizer.depthBiasSlopeFactor = Desc.RasterizationState.DepthBiasSlopeFactor;
    Rasterizer.lineWidth = Desc.RasterizationState.LineWidth;

    // 6. Multisample
    vk::PipelineMultisampleStateCreateInfo Multisampling;
    Multisampling.rasterizationSamples = ConvertSampleCount(Desc.MultisampleState.RasterizationSamples);
    Multisampling.sampleShadingEnable = Desc.MultisampleState.bSampleShadingEnable ? VK_TRUE : VK_FALSE;
    Multisampling.minSampleShading = Desc.MultisampleState.MinSampleShading;
    Multisampling.pSampleMask = &Desc.MultisampleState.SampleMask;
    Multisampling.alphaToCoverageEnable = Desc.MultisampleState.bAlphaToCoverageEnable ? VK_TRUE : VK_FALSE;
    Multisampling.alphaToOneEnable = Desc.MultisampleState.bAlphaToOneEnable ? VK_TRUE : VK_FALSE;

    // 7. Depth stencil
    vk::PipelineDepthStencilStateCreateInfo DepthStencil;
    DepthStencil.depthTestEnable = Desc.DepthStencilState.bDepthTestEnable ? VK_TRUE : VK_FALSE;
    DepthStencil.depthWriteEnable = Desc.DepthStencilState.bDepthWriteEnable ? VK_TRUE : VK_FALSE;
    DepthStencil.depthCompareOp = ConvertCompareOp(Desc.DepthStencilState.DepthCompareOp);
    DepthStencil.depthBoundsTestEnable = Desc.DepthStencilState.bDepthBoundsTestEnable ? VK_TRUE : VK_FALSE;
    DepthStencil.stencilTestEnable = Desc.DepthStencilState.bStencilTestEnable ? VK_TRUE : VK_FALSE;
    DepthStencil.front.failOp = ConvertStencilOp(Desc.DepthStencilState.FrontStencil.FailOp);
    DepthStencil.front.passOp = ConvertStencilOp(Desc.DepthStencilState.FrontStencil.PassOp);
    DepthStencil.front.depthFailOp = ConvertStencilOp(Desc.DepthStencilState.FrontStencil.DepthFailOp);
    DepthStencil.front.compareOp = ConvertCompareOp(Desc.DepthStencilState.FrontStencil.CompareOp);
    DepthStencil.front.compareMask = Desc.DepthStencilState.FrontStencil.CompareMask;
    DepthStencil.front.writeMask = Desc.DepthStencilState.FrontStencil.WriteMask;
    DepthStencil.front.reference = Desc.DepthStencilState.FrontStencil.Reference;
    DepthStencil.back.failOp = ConvertStencilOp(Desc.DepthStencilState.BackStencil.FailOp);
    DepthStencil.back.passOp = ConvertStencilOp(Desc.DepthStencilState.BackStencil.PassOp);
    DepthStencil.back.depthFailOp = ConvertStencilOp(Desc.DepthStencilState.BackStencil.DepthFailOp);
    DepthStencil.back.compareOp = ConvertCompareOp(Desc.DepthStencilState.BackStencil.CompareOp);
    DepthStencil.back.compareMask = Desc.DepthStencilState.BackStencil.CompareMask;
    DepthStencil.back.writeMask = Desc.DepthStencilState.BackStencil.WriteMask;
    DepthStencil.back.reference = Desc.DepthStencilState.BackStencil.Reference;
    DepthStencil.minDepthBounds = Desc.DepthStencilState.MinDepthBounds;
    DepthStencil.maxDepthBounds = Desc.DepthStencilState.MaxDepthBounds;

    // 8. Color blend
    TArray<vk::PipelineColorBlendAttachmentState> BlendAttachments;
    BlendAttachments.Reserve(Desc.ColorBlendState.BlendAttachments.Size());
    for (const auto& Blend : Desc.ColorBlendState.BlendAttachments)
    {
        vk::PipelineColorBlendAttachmentState VkBlend;
        VkBlend.blendEnable = Blend.bBlendEnable ? VK_TRUE : VK_FALSE;
        VkBlend.srcColorBlendFactor = ConvertBlendFactor(Blend.SrcColorBlendFactor);
        VkBlend.dstColorBlendFactor = ConvertBlendFactor(Blend.DstColorBlendFactor);
        VkBlend.colorBlendOp = ConvertBlendOp(Blend.ColorBlendOp);
        VkBlend.srcAlphaBlendFactor = ConvertBlendFactor(Blend.SrcAlphaBlendFactor);
        VkBlend.dstAlphaBlendFactor = ConvertBlendFactor(Blend.DstAlphaBlendFactor);
        VkBlend.alphaBlendOp = ConvertBlendOp(Blend.AlphaBlendOp);
        VkBlend.colorWriteMask = ConvertColorComponentFlags(Blend.ColorWriteMask);
        BlendAttachments.Add(VkBlend);
    }

    vk::PipelineColorBlendStateCreateInfo ColorBlending;
    ColorBlending.logicOpEnable = Desc.ColorBlendState.bLogicOpEnable ? VK_TRUE : VK_FALSE;
    ColorBlending.logicOp = ConvertLogicOp(Desc.ColorBlendState.LogicOp);
    ColorBlending.attachmentCount = static_cast<uint32_t>(BlendAttachments.Size());
    ColorBlending.pAttachments = BlendAttachments.Data();
    ColorBlending.blendConstants[0] = Desc.ColorBlendState.BlendConstants[0];
    ColorBlending.blendConstants[1] = Desc.ColorBlendState.BlendConstants[1];
    ColorBlending.blendConstants[2] = Desc.ColorBlendState.BlendConstants[2];
    ColorBlending.blendConstants[3] = Desc.ColorBlendState.BlendConstants[3];

    // 9. Dynamic state
    TArray<vk::DynamicState> DynamicStates = ConvertDynamicStateFlags(Desc.DynamicState.DynamicStates);

    vk::PipelineDynamicStateCreateInfo DynamicState;
    DynamicState.dynamicStateCount = static_cast<uint32_t>(DynamicStates.Size());
    DynamicState.pDynamicStates = DynamicStates.Data();

    // 10. Create graphics pipeline
    vk::GraphicsPipelineCreateInfo PipelineInfo;
    PipelineInfo.stageCount = static_cast<uint32_t>(ShaderStages.Size());
    PipelineInfo.pStages = ShaderStages.Data();
    PipelineInfo.pVertexInputState = &VertexInputInfo;
    PipelineInfo.pInputAssemblyState = &InputAssembly;
    PipelineInfo.pViewportState = &ViewportState;
    PipelineInfo.pRasterizationState = &Rasterizer;
    PipelineInfo.pMultisampleState = &Multisampling;
    PipelineInfo.pDepthStencilState = &DepthStencil;
    PipelineInfo.pColorBlendState = &ColorBlending;
    PipelineInfo.pDynamicState = DynamicStates.IsEmpty() ? nullptr : &DynamicState;
    PipelineInfo.layout = Layout;
    PipelineInfo.renderPass = Desc.RenderPass ? static_cast<VkRenderPass>(Desc.RenderPass) : VK_NULL_HANDLE;
    PipelineInfo.subpass = Desc.Subpass;

    // 使用动态渲染（如果 RenderPass 为 nullptr）
    vk::PipelineRenderingCreateInfoKHR RenderingInfo;
    if (!Desc.RenderPass)
    {
        // 注意：这里简化处理，实际应该从其他地方获取颜色附件格式等信息
        // 暂时使用空数组，后续可以扩展
        RenderingInfo.colorAttachmentCount = 0;
        RenderingInfo.pColorAttachmentFormats = nullptr;
        RenderingInfo.depthAttachmentFormat = vk::Format::eUndefined;
        RenderingInfo.stencilAttachmentFormat = vk::Format::eUndefined;

        PipelineInfo.pNext = &RenderingInfo;
    }

    try
    {
        const auto Result = Device.createGraphicsPipeline(nullptr, PipelineInfo);
        if (Result.result != vk::Result::eSuccess)
        {
            HK_LOG_FATAL(ELogcat::RHI, "创建图形管线失败: {}", vk::to_string(Result.result));
            throw std::runtime_error("创建图形管线失败");
        }
        return Result.value;
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建图形管线失败: {}", e.what());
        throw std::runtime_error((FString("创建图形管线失败: ") + FString(e.what())).CStr());
    }
}

// 内部辅助函数：创建计算管线
vk::Pipeline FGfxDeviceVk::CreateComputePipelineInternal(vk::PipelineLayout Layout, const FRHIComputePipelineDesc& Desc)
{
    HK_ASSERT_MSG_RAW(Desc.ComputeShader.IsValid(), "无效的计算着色器");

    vk::ComputePipelineCreateInfo PipelineInfo;
    PipelineInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
    PipelineInfo.stage.module = vk::ShaderModule(Desc.ComputeShader.GetHandle().Cast<VkShaderModule>());
    PipelineInfo.stage.pName = "main";
    PipelineInfo.layout = Layout;

    try
    {
        const auto Result = Device.createComputePipeline(nullptr, PipelineInfo);
        if (Result.result != vk::Result::eSuccess)
        {
            HK_LOG_FATAL(ELogcat::RHI, "创建计算管线失败: {}", vk::to_string(Result.result));
            throw std::runtime_error("创建计算管线失败");
        }
        return Result.value;
    }
    catch (const vk::SystemError& e)
    {
        HK_LOG_FATAL(ELogcat::RHI, "创建计算管线失败: {}", e.what());
        throw std::runtime_error((FString("创建计算管线失败: ") + FString(e.what())).CStr());
    }
}

void FGfxDeviceVk::DestroyPipeline(FRHIPipeline& Pipeline)
{
    if (!Pipeline.IsValid())
    {
        return;
    }

    // 获取 Vulkan 管线句柄
    const auto VulkanPipeline = vk::Pipeline(Pipeline.Handle.Cast<VkPipeline>());

    // 销毁 Vulkan 管线
    if (Device && VulkanPipeline)
    {
        Device.destroyPipeline(VulkanPipeline);
    }

    // 销毁 RHI 句柄
    auto& HandleManager = FRHIHandleManager::GetRef();
    HandleManager.DestroyRHIHandle(Pipeline.Handle);

    // 重置管线对象
    Pipeline = FRHIPipeline();

    HK_LOG_INFO(ELogcat::RHI, "管线已销毁");
}

#pragma endregion

#pragma region 转换函数实现

vk::ShaderStageFlagBits FGfxDeviceVk::ConvertShaderStage(ERHIShaderStage Stage)
{
    switch (Stage)
    {
        case ERHIShaderStage::Vertex:
            return vk::ShaderStageFlagBits::eVertex;
        case ERHIShaderStage::TessellationControl:
            return vk::ShaderStageFlagBits::eTessellationControl;
        case ERHIShaderStage::TessellationEvaluation:
            return vk::ShaderStageFlagBits::eTessellationEvaluation;
        case ERHIShaderStage::Geometry:
            return vk::ShaderStageFlagBits::eGeometry;
        case ERHIShaderStage::Fragment:
            return vk::ShaderStageFlagBits::eFragment;
        case ERHIShaderStage::Compute:
            return vk::ShaderStageFlagBits::eCompute;
        case ERHIShaderStage::Task:
            return vk::ShaderStageFlagBits::eTaskEXT;
        case ERHIShaderStage::Mesh:
            return vk::ShaderStageFlagBits::eMeshEXT;
        case ERHIShaderStage::Raygen:
            return vk::ShaderStageFlagBits::eRaygenKHR;
        case ERHIShaderStage::AnyHit:
            return vk::ShaderStageFlagBits::eAnyHitKHR;
        case ERHIShaderStage::ClosestHit:
            return vk::ShaderStageFlagBits::eClosestHitKHR;
        case ERHIShaderStage::Miss:
            return vk::ShaderStageFlagBits::eMissKHR;
        case ERHIShaderStage::Intersection:
            return vk::ShaderStageFlagBits::eIntersectionKHR;
        case ERHIShaderStage::Callable:
            return vk::ShaderStageFlagBits::eCallableKHR;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的着色器阶段: {}", static_cast<UInt32>(Stage));
            return vk::ShaderStageFlagBits::eVertex;
    }
}

vk::ShaderStageFlags FGfxDeviceVk::ConvertShaderStageFlags(ERHIShaderStage Stages)
{
    vk::ShaderStageFlags Flags = {};

    if (HasFlag(Stages, ERHIShaderStage::Vertex))
        Flags |= vk::ShaderStageFlagBits::eVertex;
    if (HasFlag(Stages, ERHIShaderStage::TessellationControl))
        Flags |= vk::ShaderStageFlagBits::eTessellationControl;
    if (HasFlag(Stages, ERHIShaderStage::TessellationEvaluation))
        Flags |= vk::ShaderStageFlagBits::eTessellationEvaluation;
    if (HasFlag(Stages, ERHIShaderStage::Geometry))
        Flags |= vk::ShaderStageFlagBits::eGeometry;
    if (HasFlag(Stages, ERHIShaderStage::Fragment))
        Flags |= vk::ShaderStageFlagBits::eFragment;
    if (HasFlag(Stages, ERHIShaderStage::Compute))
        Flags |= vk::ShaderStageFlagBits::eCompute;
    if (HasFlag(Stages, ERHIShaderStage::Task))
        Flags |= vk::ShaderStageFlagBits::eTaskEXT;
    if (HasFlag(Stages, ERHIShaderStage::Mesh))
        Flags |= vk::ShaderStageFlagBits::eMeshEXT;
    if (HasFlag(Stages, ERHIShaderStage::Raygen))
        Flags |= vk::ShaderStageFlagBits::eRaygenKHR;
    if (HasFlag(Stages, ERHIShaderStage::AnyHit))
        Flags |= vk::ShaderStageFlagBits::eAnyHitKHR;
    if (HasFlag(Stages, ERHIShaderStage::ClosestHit))
        Flags |= vk::ShaderStageFlagBits::eClosestHitKHR;
    if (HasFlag(Stages, ERHIShaderStage::Miss))
        Flags |= vk::ShaderStageFlagBits::eMissKHR;
    if (HasFlag(Stages, ERHIShaderStage::Intersection))
        Flags |= vk::ShaderStageFlagBits::eIntersectionKHR;
    if (HasFlag(Stages, ERHIShaderStage::Callable))
        Flags |= vk::ShaderStageFlagBits::eCallableKHR;

    return Flags;
}

vk::PrimitiveTopology FGfxDeviceVk::ConvertPrimitiveTopology(ERHIPrimitiveTopology Topology)
{
    switch (Topology)
    {
        case ERHIPrimitiveTopology::PointList:
            return vk::PrimitiveTopology::ePointList;
        case ERHIPrimitiveTopology::LineList:
            return vk::PrimitiveTopology::eLineList;
        case ERHIPrimitiveTopology::LineStrip:
            return vk::PrimitiveTopology::eLineStrip;
        case ERHIPrimitiveTopology::TriangleList:
            return vk::PrimitiveTopology::eTriangleList;
        case ERHIPrimitiveTopology::TriangleStrip:
            return vk::PrimitiveTopology::eTriangleStrip;
        case ERHIPrimitiveTopology::TriangleFan:
            return vk::PrimitiveTopology::eTriangleFan;
        case ERHIPrimitiveTopology::LineListWithAdjacency:
            return vk::PrimitiveTopology::eLineListWithAdjacency;
        case ERHIPrimitiveTopology::LineStripWithAdjacency:
            return vk::PrimitiveTopology::eLineStripWithAdjacency;
        case ERHIPrimitiveTopology::TriangleListWithAdjacency:
            return vk::PrimitiveTopology::eTriangleListWithAdjacency;
        case ERHIPrimitiveTopology::TriangleStripWithAdjacency:
            return vk::PrimitiveTopology::eTriangleStripWithAdjacency;
        case ERHIPrimitiveTopology::PatchList:
            return vk::PrimitiveTopology::ePatchList;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的图元拓扑: {}", static_cast<UInt32>(Topology));
            return vk::PrimitiveTopology::eTriangleList;
    }
}

vk::PolygonMode FGfxDeviceVk::ConvertPolygonMode(ERHIPolygonMode Mode)
{
    switch (Mode)
    {
        case ERHIPolygonMode::Fill:
            return vk::PolygonMode::eFill;
        case ERHIPolygonMode::Line:
            return vk::PolygonMode::eLine;
        case ERHIPolygonMode::Point:
            return vk::PolygonMode::ePoint;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的多边形模式: {}", static_cast<UInt32>(Mode));
            return vk::PolygonMode::eFill;
    }
}

vk::CullModeFlags FGfxDeviceVk::ConvertCullMode(ERHICullMode Mode)
{
    switch (Mode)
    {
        case ERHICullMode::None:
            return vk::CullModeFlagBits::eNone;
        case ERHICullMode::Front:
            return vk::CullModeFlagBits::eFront;
        case ERHICullMode::Back:
            return vk::CullModeFlagBits::eBack;
        case ERHICullMode::FrontAndBack:
            return vk::CullModeFlagBits::eFrontAndBack;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的面剔除模式: {}", static_cast<UInt32>(Mode));
            return vk::CullModeFlagBits::eBack;
    }
}

vk::FrontFace FGfxDeviceVk::ConvertFrontFace(ERHIFrontFace Face)
{
    switch (Face)
    {
        case ERHIFrontFace::CounterClockwise:
            return vk::FrontFace::eCounterClockwise;
        case ERHIFrontFace::Clockwise:
            return vk::FrontFace::eClockwise;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的前向面: {}", static_cast<UInt32>(Face));
            return vk::FrontFace::eCounterClockwise;
    }
}

vk::CompareOp FGfxDeviceVk::ConvertCompareOp(ERHICompareOp Op)
{
    switch (Op)
    {
        case ERHICompareOp::Never:
            return vk::CompareOp::eNever;
        case ERHICompareOp::Less:
            return vk::CompareOp::eLess;
        case ERHICompareOp::Equal:
            return vk::CompareOp::eEqual;
        case ERHICompareOp::LessOrEqual:
            return vk::CompareOp::eLessOrEqual;
        case ERHICompareOp::Greater:
            return vk::CompareOp::eGreater;
        case ERHICompareOp::NotEqual:
            return vk::CompareOp::eNotEqual;
        case ERHICompareOp::GreaterOrEqual:
            return vk::CompareOp::eGreaterOrEqual;
        case ERHICompareOp::Always:
            return vk::CompareOp::eAlways;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的比较操作: {}", static_cast<UInt32>(Op));
            return vk::CompareOp::eLess;
    }
}

vk::StencilOp FGfxDeviceVk::ConvertStencilOp(ERHIStencilOp Op)
{
    switch (Op)
    {
        case ERHIStencilOp::Keep:
            return vk::StencilOp::eKeep;
        case ERHIStencilOp::Zero:
            return vk::StencilOp::eZero;
        case ERHIStencilOp::Replace:
            return vk::StencilOp::eReplace;
        case ERHIStencilOp::IncrementAndClamp:
            return vk::StencilOp::eIncrementAndClamp;
        case ERHIStencilOp::DecrementAndClamp:
            return vk::StencilOp::eDecrementAndClamp;
        case ERHIStencilOp::Invert:
            return vk::StencilOp::eInvert;
        case ERHIStencilOp::IncrementAndWrap:
            return vk::StencilOp::eIncrementAndWrap;
        case ERHIStencilOp::DecrementAndWrap:
            return vk::StencilOp::eDecrementAndWrap;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的模板操作: {}", static_cast<UInt32>(Op));
            return vk::StencilOp::eKeep;
    }
}

vk::BlendFactor FGfxDeviceVk::ConvertBlendFactor(ERHIBlendFactor Factor)
{
    switch (Factor)
    {
        case ERHIBlendFactor::Zero:
            return vk::BlendFactor::eZero;
        case ERHIBlendFactor::One:
            return vk::BlendFactor::eOne;
        case ERHIBlendFactor::SrcColor:
            return vk::BlendFactor::eSrcColor;
        case ERHIBlendFactor::OneMinusSrcColor:
            return vk::BlendFactor::eOneMinusSrcColor;
        case ERHIBlendFactor::DstColor:
            return vk::BlendFactor::eDstColor;
        case ERHIBlendFactor::OneMinusDstColor:
            return vk::BlendFactor::eOneMinusDstColor;
        case ERHIBlendFactor::SrcAlpha:
            return vk::BlendFactor::eSrcAlpha;
        case ERHIBlendFactor::OneMinusSrcAlpha:
            return vk::BlendFactor::eOneMinusSrcAlpha;
        case ERHIBlendFactor::DstAlpha:
            return vk::BlendFactor::eDstAlpha;
        case ERHIBlendFactor::OneMinusDstAlpha:
            return vk::BlendFactor::eOneMinusDstAlpha;
        case ERHIBlendFactor::ConstantColor:
            return vk::BlendFactor::eConstantColor;
        case ERHIBlendFactor::OneMinusConstantColor:
            return vk::BlendFactor::eOneMinusConstantColor;
        case ERHIBlendFactor::ConstantAlpha:
            return vk::BlendFactor::eConstantAlpha;
        case ERHIBlendFactor::OneMinusConstantAlpha:
            return vk::BlendFactor::eOneMinusConstantAlpha;
        case ERHIBlendFactor::SrcAlphaSaturate:
            return vk::BlendFactor::eSrcAlphaSaturate;
        case ERHIBlendFactor::Src1Color:
            return vk::BlendFactor::eSrc1Color;
        case ERHIBlendFactor::OneMinusSrc1Color:
            return vk::BlendFactor::eOneMinusSrc1Color;
        case ERHIBlendFactor::Src1Alpha:
            return vk::BlendFactor::eSrc1Alpha;
        case ERHIBlendFactor::OneMinusSrc1Alpha:
            return vk::BlendFactor::eOneMinusSrc1Alpha;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的混合因子: {}", static_cast<UInt32>(Factor));
            return vk::BlendFactor::eOne;
    }
}

vk::BlendOp FGfxDeviceVk::ConvertBlendOp(ERHIBlendOp Op)
{
    switch (Op)
    {
        case ERHIBlendOp::Add:
            return vk::BlendOp::eAdd;
        case ERHIBlendOp::Subtract:
            return vk::BlendOp::eSubtract;
        case ERHIBlendOp::ReverseSubtract:
            return vk::BlendOp::eReverseSubtract;
        case ERHIBlendOp::Min:
            return vk::BlendOp::eMin;
        case ERHIBlendOp::Max:
            return vk::BlendOp::eMax;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的混合操作: {}", static_cast<UInt32>(Op));
            return vk::BlendOp::eAdd;
    }
}

vk::LogicOp FGfxDeviceVk::ConvertLogicOp(ERHILogicOp Op)
{
    switch (Op)
    {
        case ERHILogicOp::Clear:
            return vk::LogicOp::eClear;
        case ERHILogicOp::And:
            return vk::LogicOp::eAnd;
        case ERHILogicOp::AndReverse:
            return vk::LogicOp::eAndReverse;
        case ERHILogicOp::Copy:
            return vk::LogicOp::eCopy;
        case ERHILogicOp::AndInverted:
            return vk::LogicOp::eAndInverted;
        case ERHILogicOp::NoOp:
            return vk::LogicOp::eNoOp;
        case ERHILogicOp::Xor:
            return vk::LogicOp::eXor;
        case ERHILogicOp::Or:
            return vk::LogicOp::eOr;
        case ERHILogicOp::Nor:
            return vk::LogicOp::eNor;
        case ERHILogicOp::Equivalent:
            return vk::LogicOp::eEquivalent;
        case ERHILogicOp::Invert:
            return vk::LogicOp::eInvert;
        case ERHILogicOp::OrReverse:
            return vk::LogicOp::eOrReverse;
        case ERHILogicOp::CopyInverted:
            return vk::LogicOp::eCopyInverted;
        case ERHILogicOp::OrInverted:
            return vk::LogicOp::eOrInverted;
        case ERHILogicOp::Nand:
            return vk::LogicOp::eNand;
        case ERHILogicOp::Set:
            return vk::LogicOp::eSet;
        default:
            HK_LOG_ERROR(ELogcat::RHI, "不支持的逻辑操作: {}", static_cast<UInt32>(Op));
            return vk::LogicOp::eCopy;
    }
}

TArray<vk::DynamicState> FGfxDeviceVk::ConvertDynamicStateFlags(ERHIDynamicState States)
{
    TArray<vk::DynamicState> DynamicStates;

    if (HasFlag(States, ERHIDynamicState::Viewport))
    {
        DynamicStates.Add(vk::DynamicState::eViewport);
    }
    if (HasFlag(States, ERHIDynamicState::Scissor))
    {
        DynamicStates.Add(vk::DynamicState::eScissor);
    }
    if (HasFlag(States, ERHIDynamicState::LineWidth))
    {
        DynamicStates.Add(vk::DynamicState::eLineWidth);
    }
    if (HasFlag(States, ERHIDynamicState::DepthBias))
    {
        DynamicStates.Add(vk::DynamicState::eDepthBias);
    }
    if (HasFlag(States, ERHIDynamicState::BlendConstants))
    {
        DynamicStates.Add(vk::DynamicState::eBlendConstants);
    }
    if (HasFlag(States, ERHIDynamicState::DepthBounds))
    {
        DynamicStates.Add(vk::DynamicState::eDepthBounds);
    }
    if (HasFlag(States, ERHIDynamicState::StencilCompareMask))
    {
        DynamicStates.Add(vk::DynamicState::eStencilCompareMask);
    }
    if (HasFlag(States, ERHIDynamicState::StencilWriteMask))
    {
        DynamicStates.Add(vk::DynamicState::eStencilWriteMask);
    }
    if (HasFlag(States, ERHIDynamicState::StencilReference))
    {
        DynamicStates.Add(vk::DynamicState::eStencilReference);
    }

    return DynamicStates;
}

vk::ColorComponentFlags FGfxDeviceVk::ConvertColorComponentFlags(ERHIColorComponentFlag Flags)
{
    vk::ColorComponentFlags VkFlags = {};

    if (HasFlag(Flags, ERHIColorComponentFlag::R))
        VkFlags |= vk::ColorComponentFlagBits::eR;
    if (HasFlag(Flags, ERHIColorComponentFlag::G))
        VkFlags |= vk::ColorComponentFlagBits::eG;
    if (HasFlag(Flags, ERHIColorComponentFlag::B))
        VkFlags |= vk::ColorComponentFlagBits::eB;
    if (HasFlag(Flags, ERHIColorComponentFlag::A))
        VkFlags |= vk::ColorComponentFlagBits::eA;

    return VkFlags;
}

#pragma endregion

void FGfxDeviceVk::SetDebugName(vk::ShaderModule ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const
{
    if (!Device || Name.IsEmpty() || !bDebugUtilsExtensionAvailable || !vkSetDebugUtilsObjectNameEXT)
    {
        return;
    }

    try
    {
        const FString NameStr(Name.Data(), Name.Size());

        VkDebugUtilsObjectNameInfoEXT VkNameInfo{};
        VkNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        VkNameInfo.objectType = static_cast<VkObjectType>(ObjectType);
        VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkShaderModule>(ObjectHandle));
        VkNameInfo.pObjectName = NameStr.CStr();

        vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置ShaderModule DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}

void FGfxDeviceVk::SetDebugName(vk::PipelineLayout ObjectHandle, vk::ObjectType ObjectType,
                                const FStringView& Name) const
{
    if (!Device || Name.IsEmpty() || !bDebugUtilsExtensionAvailable || !vkSetDebugUtilsObjectNameEXT)
    {
        return;
    }

    try
    {
        const FString NameStr(Name.Data(), Name.Size());

        VkDebugUtilsObjectNameInfoEXT VkNameInfo{};
        VkNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        VkNameInfo.objectType = static_cast<VkObjectType>(ObjectType);
        VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkPipelineLayout>(ObjectHandle));
        VkNameInfo.pObjectName = NameStr.CStr();

        vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置PipelineLayout DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}

void FGfxDeviceVk::SetDebugName(vk::Pipeline ObjectHandle, vk::ObjectType ObjectType, const FStringView& Name) const
{
    if (!Device || Name.IsEmpty() || !bDebugUtilsExtensionAvailable || !vkSetDebugUtilsObjectNameEXT)
    {
        return;
    }

    try
    {
        const FString NameStr(Name.Data(), Name.Size());

        VkDebugUtilsObjectNameInfoEXT VkNameInfo{};
        VkNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        VkNameInfo.objectType = static_cast<VkObjectType>(ObjectType);
        VkNameInfo.objectHandle = reinterpret_cast<uint64_t>(static_cast<VkPipeline>(ObjectHandle));
        VkNameInfo.pObjectName = NameStr.CStr();

        vkSetDebugUtilsObjectNameEXT(static_cast<VkDevice>(Device), &VkNameInfo);
    }
    catch (const vk::SystemError& e)
    {
        // 如果扩展不可用，忽略错误（不是致命错误）
        HK_LOG_DEBUG(ELogcat::RHI, "设置Pipeline DebugName失败（扩展可能不可用）: {}", e.what());
    }
    catch (...)
    {
        // 忽略所有异常
    }
}
