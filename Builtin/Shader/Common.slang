#pragma once

public struct Camera
{
    float4x4 View;
    float4x4 Projection;
    float4x4 ExtraData1;
};

public struct Vertex_PNU
{
    float3 Position;
    float3 Normal;
    float2 UV;
};

#define DECL_CAMERA_PARAM                                                                                              \
    [[vk::binding(0, 0)]]                                                                                              \
    Camera GCamera

#define DECL_MODEL_PARAM                                                                                               \
    [[vk::binding(1, 0)]]                                                                                              \
    StructuredBuffer<float4x4> GModel

#define DECL_TEX_POOL_PARAM                                                                                            \
    [[vk::binding(2, 0)]]                                                                                              \
    Texture2D GTexturePool[]

#define DECL_SAMPLER_POOL_PARAM                                                                                        \
    [[vk::binding(2, 1)]]                                                                                              \
    SamplerState GSamplerPool[]

#define GetTexture(TextureID) GTexturePool[TextureID]
#define GetSampler(SamplerID) GSamplerPool[SamplerID]
#define GetModelMatrix(ModelID) GModel[ModelID]

// 封装 MVP 变换逻辑
float4 PerformMVP(float3 LocalPos, float4x4 ModelMatrix, Camera Cam)
{
    // 1. Local Space -> World Space
    float4 WorldPos = mul(ModelMatrix, float4(LocalPos, 1.0));

    // 2. World Space -> View Space
    float4 ViewPos = mul(Cam.View, WorldPos);

    // 3. View Space -> Clip Space
    float4 ClipPos = mul(Cam.Projection, ViewPos);

    return ClipPos;
}
