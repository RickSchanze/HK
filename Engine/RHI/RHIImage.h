#pragma once

#include "Core/Reflection/Reflection.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "Math/Vector.h"
#include "RHIHandle.h"

#include "RHIImage.generated.h"

HENUM()
enum class ERHIImageFormat : UInt32
{
    Undefined = 0,

    // 标准RGBA格式
    R8G8B8A8_UNorm = 1,   // RGBA 8-bit unorm
    R8G8B8A8_SNorm = 2,   // RGBA 8-bit snorm
    R8G8B8A8_UScaled = 3, // RGBA 8-bit uscaled
    R8G8B8A8_SScaled = 4, // RGBA 8-bit sscaled
    R8G8B8A8_UInt = 5,    // RGBA 8-bit uint
    R8G8B8A8_SInt = 6,    // RGBA 8-bit sint
    R8G8B8A8_SRGB = 7,    // RGBA 8-bit sRGB

    // BGRA格式（常用）
    B8G8R8A8_UNorm = 8, // BGRA 8-bit unorm
    B8G8R8A8_SRGB = 9,  // BGRA 8-bit sRGB

    // RGB格式
    R8G8B8_UNorm = 10,   // RGB 8-bit unorm
    R8G8B8_SNorm = 11,   // RGB 8-bit snorm
    R8G8B8_UScaled = 12, // RGB 8-bit uscaled
    R8G8B8_SScaled = 13, // RGB 8-bit sscaled
    R8G8B8_UInt = 14,    // RGB 8-bit uint
    R8G8B8_SInt = 15,    // RGB 8-bit sint
    R8G8B8_SRGB = 16,    // RGB 8-bit sRGB

    // 单通道格式
    R8_UNorm = 17,   // R 8-bit unorm
    R8_SNorm = 18,   // R 8-bit snorm
    R8_UScaled = 19, // R 8-bit uscaled
    R8_SScaled = 20, // R 8-bit sscaled
    R8_UInt = 21,    // R 8-bit uint
    R8_SInt = 22,    // R 8-bit sint

    // 16-bit格式
    R16_UNorm = 23,   // R 16-bit unorm
    R16_SNorm = 24,   // R 16-bit snorm
    R16_UScaled = 25, // R 16-bit uscaled
    R16_SScaled = 26, // R 16-bit sscaled
    R16_UInt = 27,    // R 16-bit uint
    R16_SInt = 28,    // R 16-bit sint
    R16_SFloat = 29,  // R 16-bit sfloat

    R16G16_UNorm = 30,   // RG 16-bit unorm
    R16G16_SNorm = 31,   // RG 16-bit snorm
    R16G16_UScaled = 32, // RG 16-bit uscaled
    R16G16_SScaled = 33, // RG 16-bit sscaled
    R16G16_UInt = 34,    // RG 16-bit uint
    R16G16_SInt = 35,    // RG 16-bit sint
    R16G16_SFloat = 36,  // RG 16-bit sfloat

    R16G16B16_UNorm = 37,   // RGB 16-bit unorm
    R16G16B16_SNorm = 38,   // RGB 16-bit snorm
    R16G16B16_UScaled = 39, // RGB 16-bit uscaled
    R16G16B16_SScaled = 40, // RGB 16-bit sscaled
    R16G16B16_UInt = 41,    // RGB 16-bit uint
    R16G16B16_SInt = 42,    // RGB 16-bit sint
    R16G16B16_SFloat = 43,  // RGB 16-bit sfloat

    R16G16B16A16_UNorm = 44,   // RGBA 16-bit unorm
    R16G16B16A16_SNorm = 45,   // RGBA 16-bit snorm
    R16G16B16A16_UScaled = 46, // RGBA 16-bit uscaled
    R16G16B16A16_SScaled = 47, // RGBA 16-bit sscaled
    R16G16B16A16_UInt = 48,    // RGBA 16-bit uint
    R16G16B16A16_SInt = 49,    // RGBA 16-bit sint
    R16G16B16A16_SFloat = 50,  // RGBA 16-bit sfloat

    // 32-bit格式
    R32_UInt = 51,   // R 32-bit uint
    R32_SInt = 52,   // R 32-bit sint
    R32_SFloat = 53, // R 32-bit sfloat

    R32G32_UInt = 54,   // RG 32-bit uint
    R32G32_SInt = 55,   // RG 32-bit sint
    R32G32_SFloat = 56, // RG 32-bit sfloat

    R32G32B32_UInt = 57,   // RGB 32-bit uint
    R32G32B32_SInt = 58,   // RGB 32-bit sint
    R32G32B32_SFloat = 59, // RGB 32-bit sfloat

    R32G32B32A32_UInt = 60,   // RGBA 32-bit uint
    R32G32B32A32_SInt = 61,   // RGBA 32-bit sint
    R32G32B32A32_SFloat = 62, // RGBA 32-bit sfloat

    // 深度模板格式
    D16_UNorm = 63,          // Depth 16-bit unorm
    D32_SFloat = 64,         // Depth 32-bit sfloat
    S8_UInt = 65,            // Stencil 8-bit uint
    D16_UNorm_S8_UInt = 66,  // Depth 16-bit + Stencil 8-bit
    D24_UNorm_S8_UInt = 67,  // Depth 24-bit + Stencil 8-bit
    D32_SFloat_S8_UInt = 68, // Depth 32-bit + Stencil 8-bit

    Count,
};

enum class ERHIImageUsage : UInt32
{
    None = 0,
    TransferSrc = 1 << 0,            // 可以作为传输源
    TransferDst = 1 << 1,            // 可以作为传输目标
    Sampled = 1 << 2,                // 可以被采样器访问
    Storage = 1 << 3,                // 可以作为存储图像
    ColorAttachment = 1 << 4,        // 可以作为颜色附件
    DepthStencilAttachment = 1 << 5, // 可以作为深度模板附件
    TransientAttachment = 1 << 6,    // 临时附件（在内存中可能有优化）
    InputAttachment = 1 << 7,        // 可以作为输入附件
};
HK_ENABLE_BITMASK_OPERATORS(ERHIImageUsage)

enum class ERHIImageLayout : UInt32
{
    Undefined = 0,                     // 未定义布局
    General = 1,                       // 通用布局
    ColorAttachmentOptimal = 2,        // 颜色附件最优
    DepthStencilAttachmentOptimal = 3, // 深度模板附件最优
    DepthStencilReadOnlyOptimal = 4,   // 深度模板只读最优
    ShaderReadOnlyOptimal = 5,         // 着色器只读最优
    TransferSrcOptimal = 6,            // 传输源最优
    TransferDstOptimal = 7,            // 传输目标最优
    Preinitialized = 8,                // 预初始化
    PresentSrcKHR = 9,                 // 呈现源（KHR）
};

enum class ERHIImageAspect : UInt32
{
    None = 0,
    Color = 1 << 0,   // 颜色方面
    Depth = 1 << 1,   // 深度方面
    Stencil = 1 << 2, // 模板方面
};
HK_ENABLE_BITMASK_OPERATORS(ERHIImageAspect)

enum class ERHIImageType : UInt32
{
    Image1D = 0, // 1D图像
    Image2D = 1, // 2D图像
    Image3D = 2, // 3D图像
};

enum class ERHISampleCount : UInt32
{
    Sample1 = 1,   // 1个采样点
    Sample2 = 2,   // 2个采样点
    Sample4 = 4,   // 4个采样点
    Sample8 = 8,   // 8个采样点
    Sample16 = 16, // 16个采样点
    Sample32 = 32, // 32个采样点
    Sample64 = 64, // 64个采样点
};

struct FRHIImageDesc
{
    ERHIImageType Type = ERHIImageType::Image2D;                // 图像类型
    ERHIImageFormat Format = ERHIImageFormat::R8G8B8A8_UNorm;   // 图像格式
    FVector3i Extent = {1, 1, 1};                               // 图像尺寸 (width, height, depth)
    UInt32 MipLevels = 1;                                       // MIP级别数量
    UInt32 ArrayLayers = 1;                                     // 数组层数
    ERHISampleCount Samples = ERHISampleCount::Sample1;         // 采样数量
    ERHIImageUsage Usage = ERHIImageUsage::None;                // 使用标志
    ERHIImageLayout InitialLayout = ERHIImageLayout::Undefined; // 初始布局
    FString DebugName;                                          // 调试名称
};

// 简单的 Image 值类型，只包含一个 Handle
class FRHIImage
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 Image（无效）
    FRHIImage() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::DestroyImage 销毁
    ~FRHIImage() = default;

    // 允许拷贝和移动
    FRHIImage(const FRHIImage& Other) = default;
    FRHIImage& operator=(const FRHIImage& Other) = default;
    FRHIImage(FRHIImage&& Other) noexcept = default;
    FRHIImage& operator=(FRHIImage&& Other) noexcept = default;

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

    // 获取图像类型
    ERHIImageType GetType() const
    {
        return Type;
    }

    // 获取图像格式
    ERHIImageFormat GetFormat() const
    {
        return Format;
    }

    // 获取图像尺寸
    const FVector3i& GetExtent() const
    {
        return Extent;
    }

    // 获取MIP级别数量
    UInt32 GetMipLevels() const
    {
        return MipLevels;
    }

    // 获取数组层数
    UInt32 GetArrayLayers() const
    {
        return ArrayLayers;
    }

    // 获取采样数量
    ERHISampleCount GetSamples() const
    {
        return Samples;
    }

    // 获取使用标志
    ERHIImageUsage GetUsage() const
    {
        return Usage;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHIImage& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHIImage& Other) const
    {
        return Handle != Other.Handle;
    }

private:
    FRHIHandle Handle;
    ERHIImageType Type = ERHIImageType::Image2D;
    ERHIImageFormat Format = ERHIImageFormat::Undefined;
    FVector3i Extent = {0, 0, 0};
    UInt32 MipLevels = 0;
    UInt32 ArrayLayers = 0;
    ERHISampleCount Samples = ERHISampleCount::Sample1;
    ERHIImageUsage Usage = ERHIImageUsage::None;
};
