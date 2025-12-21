#pragma once

#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "RHIHandle.h"
#include "RHIImage.h"

struct FRHIImageViewDesc
{
    ERHIImageType ViewType = ERHIImageType::Image2D;          // 视图类型
    ERHIImageFormat Format = ERHIImageFormat::R8G8B8A8_UNorm; // 视图格式（通常与图像相同）
    ERHIImageAspect Aspects = ERHIImageAspect::Color;         // 图像方面
    UInt32 BaseMipLevel = 0;                                  // 基础MIP级别
    UInt32 LevelCount = 1;                                    // MIP级别数量
    UInt32 BaseArrayLayer = 0;                                // 基础数组层
    UInt32 LayerCount = 1;                                    // 数组层数量
    FString DebugName;                                        // 调试名称
};

// 图像视图类
class FRHIImageView
{
    friend class FGfxDevice;
    friend class FGfxDeviceVk;

public:
    // 默认构造：创建空的 ImageView（无效）
    FRHIImageView() = default;

    // 析构函数：不自动销毁资源，必须通过 FRHIDevice::DestroyImageView 销毁
    ~FRHIImageView() = default;

    // 允许拷贝和移动
    FRHIImageView(const FRHIImageView& Other) = default;
    FRHIImageView& operator=(const FRHIImageView& Other) = default;
    FRHIImageView(FRHIImageView&& Other) noexcept = default;
    FRHIImageView& operator=(FRHIImageView&& Other) noexcept = default;

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

    // 获取关联的图像
    const FRHIImage* GetImage() const
    {
        return Image;
    }

    // 获取视图类型
    ERHIImageType GetViewType() const
    {
        return ViewType;
    }

    // 获取视图格式
    ERHIImageFormat GetFormat() const
    {
        return Format;
    }

    // 获取图像方面
    ERHIImageAspect GetAspects() const
    {
        return Aspects;
    }

    operator bool() const
    {
        return IsValid();
    }

    // 比较操作符
    bool operator==(const FRHIImageView& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FRHIImageView& Other) const
    {
        return Handle != Other.Handle;
    }

private:
    FRHIHandle Handle;
    const FRHIImage* Image = nullptr; // 关联的图像（弱引用）
    ERHIImageType ViewType = ERHIImageType::Image2D;
    ERHIImageFormat Format = ERHIImageFormat::Undefined;
    ERHIImageAspect Aspects = ERHIImageAspect::None;
};
