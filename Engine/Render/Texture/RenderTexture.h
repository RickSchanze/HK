#pragma once

#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "Math/Vector.h"
#include "RHI/RHIImage.h"
#include "RHI/RHIImageView.h"

/**
 * RenderTexture - 用于渲染的纹理类
 * 可以作为颜色附件或深度/模板附件使用
 */
class HK_API FRenderTexture
{
public:
    /**
     * 构造函数 - 创建RenderTexture
     * @param InWidth 宽度
     * @param InHeight 高度
     * @param InFormat 图像格式 (默认: R8G8B8A8_UNorm，用于颜色渲染)
     * @param InUsage 图像用途 (默认: ColorAttachment | Sampled，用于颜色渲染与采样)
     * @param InDebugName 调试名称
     */
    FRenderTexture(UInt32 InWidth, UInt32 InHeight, ERHIImageFormat InFormat = ERHIImageFormat::R8G8B8A8_UNorm,
                   ERHIImageUsage InUsage     = ERHIImageUsage::ColorAttachment | ERHIImageUsage::Sampled,
                   FStringView    InDebugName = "RenderTexture");

    /**
     * 析构函数 - 不自动释放资源
     * 需要手动调用 Release() 来释放资源
     */
    ~FRenderTexture();

    // 禁止拷贝
    FRenderTexture(const FRenderTexture&)            = delete;
    FRenderTexture& operator=(const FRenderTexture&) = delete;

    // 允许移动
    FRenderTexture(FRenderTexture&& Other) noexcept;
    FRenderTexture& operator=(FRenderTexture&& Other) noexcept;

    /**
     * 调整RenderTexture大小
     * @param NewWidth 新宽度
     * @param NewHeight 新高度
     * @return 是否成功调整大小
     */
    bool Resize(UInt32 NewWidth, UInt32 NewHeight);

    /**
     * 释放RenderTexture资源
     * 必须手动调用此方法来释放GPU资源
     */
    void Release();

    /**
     * 检查RenderTexture是否有效
     * @return 如果已创建且有效则返回true
     */
    bool IsValid() const
    {
        return Image.IsValid() && ImageView.IsValid();
    }

    // Getter方法
    UInt32 GetWidth() const
    {
        return Width;
    }

    UInt32 GetHeight() const
    {
        return Height;
    }

    FVector2i GetSize() const
    {
        return FVector2i(static_cast<Int32>(Width), static_cast<Int32>(Height));
    }

    ERHIImageFormat GetFormat() const
    {
        return Format;
    }

    ERHIImageUsage GetUsage() const
    {
        return Usage;
    }

    const FRHIImage& GetRHIImage() const
    {
        return Image;
    }

    const FRHIImageView& GetRHIImageView() const
    {
        return ImageView;
    }

    const FString& GetDebugName() const
    {
        return DebugName;
    }

    /**
     * 检查是否为深度相关格式（深度或深度/模板）
     * @return 如果是深度相关格式则返回true
     */
    bool IsDepthFormat() const;

    /**
     * 检查是否为模板格式
     * @return 如果是模板格式则返回true
     */
    bool IsStencilFormat() const;

    /**
     * 获取图像方面标志（Color、Depth、Stencil）
     * @return 图像方面标志
     */
    ERHIImageAspect GetImageAspect() const;

private:
    /**
     * 创建RenderTexture资源
     * @return 是否成功创建
     */
    bool CreateResources();

    UInt32          Width     = 0;
    UInt32          Height    = 0;
    ERHIImageFormat Format    = ERHIImageFormat::R8G8B8A8_UNorm;
    ERHIImageUsage  Usage     = ERHIImageUsage::ColorAttachment | ERHIImageUsage::Sampled;
    FString         DebugName = "RenderTexture";
    FRHIImage       Image;
    FRHIImageView   ImageView;
};
