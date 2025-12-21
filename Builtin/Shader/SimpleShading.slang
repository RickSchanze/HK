#include "Common.slang"

DECL_CAMERA_PARAM;
DECL_MODEL_PARAM;
DECL_TEX_POOL_PARAM;
DECL_SAMPLER_POOL_PARAM;

// [新增] 定义 PushConstants 结构
struct InstancePushConstants
{
    uint ModelID;            // 用于去 GModel 查矩阵
    uint MainTextureID;      // 用于去 GTexturePool 查纹理
    uint MainSamplerStateID; // 用于去 GSamplerPool 查采样器
};
// 定义 PushConstant (不需要 binding 号，因为它不是 DescriptorSet 的一部分)
[[vk::push_constant]]
ConstantBuffer<InstancePushConstants> GPushConstants;

// 顶点着色器输出 / 像素着色器输入
struct VSOut
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
};

// 像素着色器输出
struct FSOut
{
    float4 Color : SV_TARGET;
};

// -----------------------------------------------------------
// Vertex Shader
// -----------------------------------------------------------
[shader("vertex")]
VSOut vertex(Vertex_PNU In)
{
    VSOut Out;

    // 1. 从 SSBO 获取当前实例的模型矩阵
    float4x4 ModelMatrix = GModel[GPushConstants.ModelID];

    Out.Position = PerformMVP(In.Position, ModelMatrix, GCamera);

    // 4. 传递 UV 和 ID
    Out.UV = In.UV;

    return Out;
}

// -----------------------------------------------------------
// Fragment Shader
// -----------------------------------------------------------
[shader("fragment")]
FSOut fragment(VSOut In)
{
    FSOut Out;

    // 1. 确定要使用的资源索引
    // 由于 GModel 只存了矩阵，这里演示直接用 InstanceID 作为纹理索引
    // 实际项目中，你通常会把 TextureID 打包进 GModel 或者另一个 Buffer
    uint TexIndex = GPushConstants.MainTextureID;

    // 假设所有物体共用 0 号采样器 (Linear)
    uint SamplerIndex = 0;

    // 2. Bindless 纹理采样
    // 关键：必须使用 NonUniformResourceIndex，因为不同的 Instance 可能访问不同的纹理
    Texture2D TargetTex = GTexturePool[NonUniformResourceIndex(TexIndex)];
    SamplerState TargetSampler = GSamplerPool[SamplerIndex];

    // 3. 采样并输出
    float4 color = TargetTex.Sample(TargetSampler, In.UV);

    Out.Color = color;

    return Out;
}
