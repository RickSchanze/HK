#pragma once

#include "Core/Container/Array.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "Math/Vector.h"
#include "RHIDescriptorSet.h"
#include "RHIHandle.h"
#include "RHIImage.h"

enum class ERHIShaderStage : UInt32
{
    Vertex = 1 << 0,                 // 顶点着色器
    TessellationControl = 1 << 1,    // 曲面细分控制着色器
    TessellationEvaluation = 1 << 2, // 曲面细分计算着色器
    Geometry = 1 << 3,               // 几何着色器
    Fragment = 1 << 4,               // 片段着色器
    Compute = 1 << 5,                // 计算着色器
    Task = 1 << 6,                   // 任务着色器（网格着色器）
    Mesh = 1 << 7,                   // 网格着色器
    Raygen = 1 << 8,                 // 光线生成着色器（光线追踪）
    AnyHit = 1 << 9,                 // 任意命中着色器（光线追踪）
    ClosestHit = 1 << 10,            // 最近命中着色器（光线追踪）
    Miss = 1 << 11,                  // 未命中着色器（光线追踪）
    Intersection = 1 << 12,          // 相交着色器（光线追踪）
    Callable = 1 << 13,              // 可调用着色器（光线追踪）
};
HK_ENABLE_BITMASK_OPERATORS(ERHIShaderStage)

struct FRHIShaderModuleDesc
{
    TArray<UInt32> Code; // SPIR-V 代码（UInt32 数组）
    FString DebugName;   // 调试名称
};

// 着色器模块类
class FRHIShaderModule
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 ShaderModule（无效）
    FRHIShaderModule() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::DestroyShaderModule 销毁
    ~FRHIShaderModule() = default;

    // 允许拷贝和移动
    FRHIShaderModule(const FRHIShaderModule& Other) = default;
    FRHIShaderModule& operator=(const FRHIShaderModule& Other) = default;
    FRHIShaderModule(FRHIShaderModule&& Other) noexcept = default;
    FRHIShaderModule& operator=(FRHIShaderModule&& Other) noexcept = default;

    // 检查是否有效
    bool IsValid() const
    {
        return Handle.IsValid();
    }

    // 获取底层句柄
    const FRHIHandle& GetHandle() const
    {
        return Handle;
    }

    FRHIHandle& GetHandle()
    {
        return Handle;
    }

    // 获取着色器阶段
    ERHIShaderStage GetStage() const
    {
        return Stage;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHIShaderModule& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHIShaderModule& Other) const
    {
        return Handle != Other.Handle;
    }

private:
    FRHIHandle Handle;
    ERHIShaderStage Stage = ERHIShaderStage::Vertex;
};

struct FRHIPipelineLayoutDesc
{
    TArray<FRHIDescriptorSetLayout> SetLayouts; // 描述符集布局数组
    TArray<UInt32> PushConstantRanges; // 推送常量范围（每4个UInt32表示一个范围：offset, size, stageFlags, 保留）
    FString DebugName;                 // 调试名称
};

// 管线布局类
class FRHIPipelineLayout
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 PipelineLayout（无效）
    FRHIPipelineLayout() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::DestroyPipelineLayout 销毁
    ~FRHIPipelineLayout() = default;

    // 允许拷贝和移动
    FRHIPipelineLayout(const FRHIPipelineLayout& Other) = default;
    FRHIPipelineLayout& operator=(const FRHIPipelineLayout& Other) = default;
    FRHIPipelineLayout(FRHIPipelineLayout&& Other) noexcept = default;
    FRHIPipelineLayout& operator=(FRHIPipelineLayout&& Other) noexcept = default;

    // 检查是否有效
    bool IsValid() const
    {
        return Handle.IsValid();
    }

    // 获取底层句柄
    const FRHIHandle& GetHandle() const
    {
        return Handle;
    }

    FRHIHandle& GetHandle()
    {
        return Handle;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHIPipelineLayout& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHIPipelineLayout& Other) const
    {
        return Handle != Other.Handle;
    }

private:
    FRHIHandle Handle;
};

enum class ERHIPipelineType : UInt32
{
    Graphics = 0,   // 图形管线
    Compute = 1,    // 计算管线
    RayTracing = 2, // 光线追踪管线
};

enum class ERHIPrimitiveTopology : UInt32
{
    PointList = 0,
    LineList = 1,
    LineStrip = 2,
    TriangleList = 3,
    TriangleStrip = 4,
    TriangleFan = 5,
    LineListWithAdjacency = 6,
    LineStripWithAdjacency = 7,
    TriangleListWithAdjacency = 8,
    TriangleStripWithAdjacency = 9,
    PatchList = 10,
};

enum class ERHIPolygonMode : UInt32
{
    Fill = 0,
    Line = 1,
    Point = 2,
};

enum class ERHICullMode : UInt32
{
    None = 0,
    Front = 1,
    Back = 2,
    FrontAndBack = 3,
};

enum class ERHIFrontFace : UInt32
{
    CounterClockwise = 0,
    Clockwise = 1,
};

enum class ERHICompareOp : UInt32
{
    Never = 0,
    Less = 1,
    Equal = 2,
    LessOrEqual = 3,
    Greater = 4,
    NotEqual = 5,
    GreaterOrEqual = 6,
    Always = 7,
};

enum class ERHIStencilOp : UInt32
{
    Keep = 0,
    Zero = 1,
    Replace = 2,
    IncrementAndClamp = 3,
    DecrementAndClamp = 4,
    Invert = 5,
    IncrementAndWrap = 6,
    DecrementAndWrap = 7,
};

enum class ERHIBlendFactor : UInt32
{
    Zero = 0,
    One = 1,
    SrcColor = 2,
    OneMinusSrcColor = 3,
    DstColor = 4,
    OneMinusDstColor = 5,
    SrcAlpha = 6,
    OneMinusSrcAlpha = 7,
    DstAlpha = 8,
    OneMinusDstAlpha = 9,
    ConstantColor = 10,
    OneMinusConstantColor = 11,
    ConstantAlpha = 12,
    OneMinusConstantAlpha = 13,
    SrcAlphaSaturate = 14,
    Src1Color = 15,
    OneMinusSrc1Color = 16,
    Src1Alpha = 17,
    OneMinusSrc1Alpha = 18,
};

enum class ERHIBlendOp : UInt32
{
    Add = 0,
    Subtract = 1,
    ReverseSubtract = 2,
    Min = 3,
    Max = 4,
};

enum class ERHILogicOp : UInt32
{
    Clear = 0,
    And = 1,
    AndReverse = 2,
    Copy = 3,
    AndInverted = 4,
    NoOp = 5,
    Xor = 6,
    Or = 7,
    Nor = 8,
    Equivalent = 9,
    Invert = 10,
    OrReverse = 11,
    CopyInverted = 12,
    OrInverted = 13,
    Nand = 14,
    Set = 15,
};

enum class ERHIColorComponentFlag : UInt32
{
    None = 0,
    R = 1 << 0,
    G = 1 << 1,
    B = 1 << 2,
    A = 1 << 3,
};
HK_ENABLE_BITMASK_OPERATORS(ERHIColorComponentFlag)

enum class ERHIDynamicState : UInt32
{
    None = 0,
    Viewport = 1 << 0,
    Scissor = 1 << 1,
    LineWidth = 1 << 2,
    DepthBias = 1 << 3,
    BlendConstants = 1 << 4,
    DepthBounds = 1 << 5,
    StencilCompareMask = 1 << 6,
    StencilWriteMask = 1 << 7,
    StencilReference = 1 << 8,
};
HK_ENABLE_BITMASK_OPERATORS(ERHIDynamicState)

struct FRHIVertexInputBindingDescription
{
    UInt32 Binding = 0;         // 绑定索引
    UInt32 Stride = 0;          // 顶点数据步长（字节）
    bool bInstanceRate = false; // 是否为实例速率
};

struct FRHIVertexInputAttributeDescription
{
    UInt32 Location = 0;                                           // 位置（对应着色器中的 location）
    UInt32 Binding = 0;                                            // 绑定索引
    ERHIImageFormat Format = ERHIImageFormat::R32G32B32A32_SFloat; // 属性格式
    UInt32 Offset = 0;                                             // 偏移量（字节）
};

struct FRHIViewport
{
    float X = 0.0f;
    float Y = 0.0f;
    float Width = 0.0f;
    float Height = 0.0f;
    float MinDepth = 0.0f;
    float MaxDepth = 1.0f;
};

struct FRHIRect2D
{
    Int32 X = 0;
    Int32 Y = 0;
    UInt32 Width = 0;
    UInt32 Height = 0;
};

struct FRHIStencilOpState
{
    ERHIStencilOp FailOp = ERHIStencilOp::Keep;
    ERHIStencilOp PassOp = ERHIStencilOp::Keep;
    ERHIStencilOp DepthFailOp = ERHIStencilOp::Keep;
    ERHICompareOp CompareOp = ERHICompareOp::Always;
    UInt32 CompareMask = 0xFF;
    UInt32 WriteMask = 0xFF;
    UInt32 Reference = 0;
};

struct FRHIBlendAttachmentState
{
    bool bBlendEnable = false;
    ERHIBlendFactor SrcColorBlendFactor = ERHIBlendFactor::One;
    ERHIBlendFactor DstColorBlendFactor = ERHIBlendFactor::Zero;
    ERHIBlendOp ColorBlendOp = ERHIBlendOp::Add;
    ERHIBlendFactor SrcAlphaBlendFactor = ERHIBlendFactor::One;
    ERHIBlendFactor DstAlphaBlendFactor = ERHIBlendFactor::Zero;
    ERHIBlendOp AlphaBlendOp = ERHIBlendOp::Add;
    ERHIColorComponentFlag ColorWriteMask = ERHIColorComponentFlag::R | ERHIColorComponentFlag::G |
                                            ERHIColorComponentFlag::B | ERHIColorComponentFlag::A; // 颜色写入掩码
};

struct FRHIPipelineShaderStageState
{
    TArray<FRHIShaderModule> ShaderModules; // 着色器模块数组
};

struct FRHIPipelineVertexInputState
{
    TArray<FRHIVertexInputBindingDescription> VertexBindings;
    TArray<FRHIVertexInputAttributeDescription> VertexAttributes;
};

struct FRHIPipelineInputAssemblyState
{
    ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;
    bool bPrimitiveRestartEnable = false;
};

struct FRHIPipelineViewportState
{
    TArray<FRHIViewport> Viewports;
    TArray<FRHIRect2D> Scissors;
};

struct FRHIPipelineRasterizationState
{
    bool bDepthClampEnable = false;
    bool bRasterizerDiscardEnable = false;
    ERHIPolygonMode PolygonMode = ERHIPolygonMode::Fill;
    ERHICullMode CullMode = ERHICullMode::Back;
    ERHIFrontFace FrontFace = ERHIFrontFace::CounterClockwise;
    bool bDepthBiasEnable = false;
    float DepthBiasConstantFactor = 0.0f;
    float DepthBiasClamp = 0.0f;
    float DepthBiasSlopeFactor = 0.0f;
    float LineWidth = 1.0f;
};

struct FRHIPipelineMultisampleState
{
    ERHISampleCount RasterizationSamples = ERHISampleCount::Sample1;
    bool bSampleShadingEnable = false;
    float MinSampleShading = 0.0f;
    UInt32 SampleMask = 0xFFFFFFFF;
    bool bAlphaToCoverageEnable = false;
    bool bAlphaToOneEnable = false;
};

struct FRHIPipelineDepthStencilState
{
    bool bDepthTestEnable = true;
    bool bDepthWriteEnable = true;
    ERHICompareOp DepthCompareOp = ERHICompareOp::Less;
    bool bDepthBoundsTestEnable = false;
    bool bStencilTestEnable = false;
    FRHIStencilOpState FrontStencil;
    FRHIStencilOpState BackStencil;
    float MinDepthBounds = 0.0f;
    float MaxDepthBounds = 1.0f;
};

struct FRHIPipelineColorBlendState
{
    bool bLogicOpEnable = false;
    ERHILogicOp LogicOp = ERHILogicOp::Copy;
    TArray<FRHIBlendAttachmentState> BlendAttachments;
    float BlendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};

struct FRHIPipelineDynamicState
{
    ERHIDynamicState DynamicStates = ERHIDynamicState::None;
};

struct FRHIGraphicsPipelineDesc
{
    FRHIPipelineLayout Layout; // 管线布局（必须有效）
    FString DebugName;         // 调试名称

    // Render pass (nullptr 表示使用 dynamic rendering)
    void* RenderPass = nullptr; // 暂时使用 void*，后续改为 FRHIRenderPass
    UInt32 Subpass = 0;

    // Pipeline state (类似 Vulkan 的组织方式)
    FRHIPipelineShaderStageState ShaderStageState;
    FRHIPipelineVertexInputState VertexInputState;
    FRHIPipelineInputAssemblyState InputAssemblyState;
    FRHIPipelineViewportState ViewportState;
    FRHIPipelineRasterizationState RasterizationState;
    FRHIPipelineMultisampleState MultisampleState;
    FRHIPipelineDepthStencilState DepthStencilState;
    FRHIPipelineColorBlendState ColorBlendState;
    FRHIPipelineDynamicState DynamicState;
};

struct FRHIComputePipelineDesc
{
    FRHIPipelineLayout Layout;      // 管线布局（必须有效）
    FRHIShaderModule ComputeShader; // 计算着色器（必须有效）
    FString DebugName;              // 调试名称
};

struct FRHIRayTracingPipelineDesc
{
    FRHIPipelineLayout Layout;              // 管线布局（必须有效）
    TArray<FRHIShaderModule> ShaderModules; // 着色器模块数组
    TArray<UInt32> ShaderGroupIndices;      // 着色器组索引（每5个UInt32表示一个组：type, generalIndex, closestHitIndex,
                                            // anyHitIndex, intersectionIndex）
    UInt32 MaxRayRecursionDepth = 1;        // 最大光线递归深度
    FString DebugName;                      // 调试名称
};

// 管线类
class FRHIPipeline
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 Pipeline（无效）
    FRHIPipeline() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::DestroyPipeline 销毁
    ~FRHIPipeline() = default;

    // 允许拷贝和移动
    FRHIPipeline(const FRHIPipeline& Other) = default;
    FRHIPipeline& operator=(const FRHIPipeline& Other) = default;
    FRHIPipeline(FRHIPipeline&& Other) noexcept = default;
    FRHIPipeline& operator=(FRHIPipeline&& Other) noexcept = default;

    // 检查是否有效
    bool IsValid() const
    {
        return Handle.IsValid();
    }

    // 获取底层句柄
    const FRHIHandle& GetHandle() const
    {
        return Handle;
    }

    FRHIHandle& GetHandle()
    {
        return Handle;
    }

    // 获取管线类型
    ERHIPipelineType GetType() const
    {
        return Type;
    }

    // 获取管线布局
    const FRHIPipelineLayout& GetLayout() const
    {
        return Layout;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHIPipeline& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHIPipeline& Other) const
    {
        return Handle != Other.Handle;
    }

private:
    FRHIHandle Handle;
    ERHIPipelineType Type = ERHIPipelineType::Graphics;
    FRHIPipelineLayout Layout; // 关联的管线布局
};
