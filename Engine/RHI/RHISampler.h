#pragma once

#include "Core/String/String.h"
#include "Core/Utility/HashUtility.h"
#include "Core/Utility/Macros.h"
#include "RHIHandle.h"
#include "RHIImage.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "RHISampler.generated.h"

HENUM()
enum class ERHIFilter : UInt32
{
    Nearest = 0, // 最近邻过滤
    Linear  = 1, // 线性过滤
};

HENUM()
enum class ERHISamplerAddressMode : UInt32
{
    Repeat            = 0, // 重复
    MirroredRepeat    = 1, // 镜像重复
    ClampToEdge       = 2, // 钳制到边缘
    ClampToBorder     = 3, // 钳制到边界
    MirrorClampToEdge = 4, // 镜像钳制到边缘
};

HENUM()
enum class ERHISamplerBorderColor : UInt32
{
    FloatTransparentBlack = 0, // 浮点透明黑
    IntTransparentBlack   = 1, // 整数透明黑
    FloatOpaqueBlack      = 2, // 浮点不透明黑
    IntOpaqueBlack        = 3, // 整数不透明黑
    FloatOpaqueWhite      = 4, // 浮点不透明白
    IntOpaqueWhite        = 5, // 整数不透明白
};

HENUM()
enum class ERHISamplerMipmapMode : UInt32
{
    Nearest = 0, // 最近邻 MIP 映射
    Linear  = 1, // 线性 MIP 映射
};

struct FRHISamplerDesc
{
    ERHIFilter             MagFilter                = ERHIFilter::Linear;             // 放大过滤
    ERHIFilter             MinFilter                = ERHIFilter::Linear;             // 缩小过滤
    ERHISamplerMipmapMode  MipmapMode               = ERHISamplerMipmapMode::Linear;  // MIP 映射模式
    ERHISamplerAddressMode AddressModeU             = ERHISamplerAddressMode::Repeat; // U 方向地址模式
    ERHISamplerAddressMode AddressModeV             = ERHISamplerAddressMode::Repeat; // V 方向地址模式
    ERHISamplerAddressMode AddressModeW             = ERHISamplerAddressMode::Repeat; // W 方向地址模式
    float                  MipLodBias               = 0.0f;                           // MIP LOD 偏移
    bool                   bAnisotropyEnable        = false;                          // 是否启用各向异性过滤
    float                  MaxAnisotropy            = 1.0f;                           // 最大各向异性值
    bool                   bCompareEnable           = false;                          // 是否启用比较
    ERHICompareOp          CompareOp                = ERHICompareOp::Never;           // 比较操作
    float                  MinLod                   = 0.0f;                           // 最小 LOD
    float                  MaxLod                   = 0.0f; // 最大 LOD（0.0 表示使用所有 MIP 级别）
    ERHISamplerBorderColor BorderColor              = ERHISamplerBorderColor::FloatTransparentBlack; // 边界颜色
    bool                   bUnnormalizedCoordinates = false; // 是否使用非标准化坐标
    FString                DebugName;                        // 调试名称

    UInt64 GetHashCode() const
    {
        return FHashUtility::CombineHashes(
            std::hash<UInt32>{}(static_cast<UInt32>(MagFilter)), std::hash<UInt32>{}(static_cast<UInt32>(MinFilter)),
            std::hash<UInt32>{}(static_cast<UInt32>(MipmapMode)),
            std::hash<UInt32>{}(static_cast<UInt32>(AddressModeU)),
            std::hash<UInt32>{}(static_cast<UInt32>(AddressModeV)),
            std::hash<UInt32>{}(static_cast<UInt32>(AddressModeW)), std::hash<float>{}(MipLodBias),
            std::hash<bool>{}(bAnisotropyEnable), std::hash<float>{}(MaxAnisotropy), std::hash<bool>{}(bCompareEnable),
            std::hash<UInt32>{}(static_cast<UInt32>(CompareOp)), std::hash<float>{}(MinLod), std::hash<float>{}(MaxLod),
            std::hash<UInt32>{}(static_cast<UInt32>(BorderColor)), std::hash<bool>{}(bUnnormalizedCoordinates));
    }
};

// 采样器类
class FRHISampler
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 Sampler（无效）
    FRHISampler() = default;

    // 析构函数：不自动销毁资源，必须通过 FGfxDevice::DestroySampler 销毁
    ~FRHISampler() = default;

    // 允许拷贝和移动
    FRHISampler(const FRHISampler& Other)                = default;
    FRHISampler& operator=(const FRHISampler& Other)     = default;
    FRHISampler(FRHISampler&& Other) noexcept            = default;
    FRHISampler& operator=(FRHISampler&& Other) noexcept = default;

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

    // 获取放大过滤
    ERHIFilter GetMagFilter() const
    {
        return MagFilter;
    }

    // 获取缩小过滤
    ERHIFilter GetMinFilter() const
    {
        return MinFilter;
    }

    // 获取 MIP 映射模式
    ERHISamplerMipmapMode GetMipmapMode() const
    {
        return MipmapMode;
    }

    // 获取地址模式
    ERHISamplerAddressMode GetAddressModeU() const
    {
        return AddressModeU;
    }

    ERHISamplerAddressMode GetAddressModeV() const
    {
        return AddressModeV;
    }

    ERHISamplerAddressMode GetAddressModeW() const
    {
        return AddressModeW;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHISampler& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHISampler& Other) const
    {
        return Handle != Other.Handle;
    }

    UInt64 GetHashCode() const
    {
        return Handle.GetHashCode();
    }

private:
    FRHIHandle             Handle;
    ERHIFilter             MagFilter    = ERHIFilter::Linear;
    ERHIFilter             MinFilter    = ERHIFilter::Linear;
    ERHISamplerMipmapMode  MipmapMode   = ERHISamplerMipmapMode::Linear;
    ERHISamplerAddressMode AddressModeU = ERHISamplerAddressMode::Repeat;
    ERHISamplerAddressMode AddressModeV = ERHISamplerAddressMode::Repeat;
    ERHISamplerAddressMode AddressModeW = ERHISamplerAddressMode::Repeat;
};
