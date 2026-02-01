#include "RenderTexture.h"

#include "Core/Logging/Logger.h"
#include "RHI/GfxDevice.h"

FRenderTexture::FRenderTexture(UInt32 InWidth, UInt32 InHeight, ERHIImageFormat InFormat, ERHIImageUsage InUsage,
                               FStringView InDebugName)
    : Width(InWidth), Height(InHeight), Format(InFormat), Usage(InUsage), DebugName(InDebugName), bOwnsImageView(true)
{
    // 如果参数合规，直接创建资源
    if (Width > 0 && Height > 0 && Format != ERHIImageFormat::Undefined)
    {
        if (!CreateResources())
        {
            HK_LOG_ERROR(ELogcat::Render, "Failed to create RenderTexture: {}", DebugName);
        }
    }
    else
    {
        HK_LOG_WARN(ELogcat::Render, "RenderTexture created with invalid parameters: {}. Width: {}, Height: {}",
                    DebugName, Width, Height);
    }
}

FRenderTexture::FRenderTexture(const FRHIImageView& InImageView, UInt32 InWidth, UInt32 InHeight,
                               ERHIImageFormat InFormat, FStringView InDebugName)
    : Width(InWidth), Height(InHeight), Format(InFormat), DebugName(InDebugName), ImageView(InImageView),
      bOwnsImageView(false)
{
    // 外部ImageView模式：不创建自己的Image和ImageView
    // ImageView由外部管理（例如SwapChain），不需要释放
    if (!ImageView.IsValid())
    {
        HK_LOG_WARN(ELogcat::Render, "RenderTexture '{}' created with invalid external ImageView", DebugName);
    }
}

FRenderTexture::~FRenderTexture()
{
    // 析构函数不自动释放资源
    // 用户必须手动调用 Release()
    if (IsValid())
    {
        HK_LOG_WARN(ELogcat::Render,
                    "RenderTexture '{}' is being destroyed without calling Release(). Memory leak may occur.",
                    DebugName);
    }
}

FRenderTexture::FRenderTexture(FRenderTexture&& Other) noexcept
    : Width(Other.Width), Height(Other.Height), Format(Other.Format), Usage(Other.Usage),
      DebugName(std::move(Other.DebugName)), Image(std::move(Other.Image)), ImageView(std::move(Other.ImageView)),
      bOwnsImageView(Other.bOwnsImageView)
{
    Other.Width          = 0;
    Other.Height         = 0;
    Other.Format         = ERHIImageFormat::Undefined;
    Other.Usage          = ERHIImageUsage::None;
    Other.bOwnsImageView = true;  // 重置为默认值
}

FRenderTexture& FRenderTexture::operator=(FRenderTexture&& Other) noexcept
{
    if (this != &Other)
    {
        // 释放当前资源
        Release();

        // 移动数据
        Width          = Other.Width;
        Height         = Other.Height;
        Format         = Other.Format;
        Usage          = Other.Usage;
        DebugName      = std::move(Other.DebugName);
        Image          = std::move(Other.Image);
        ImageView      = std::move(Other.ImageView);
        bOwnsImageView = Other.bOwnsImageView;

        // 清空源对象
        Other.Width          = 0;
        Other.Height         = 0;
        Other.Format         = ERHIImageFormat::Undefined;
        Other.Usage          = ERHIImageUsage::None;
        Other.bOwnsImageView = true; // 重置为默认值
    }
    return *this;
}

bool FRenderTexture::Resize(UInt32 NewWidth, UInt32 NewHeight)
{
    if (NewWidth == 0 || NewHeight == 0)
    {
        HK_LOG_ERROR(ELogcat::Render, "Cannot resize RenderTexture '{}' to invalid dimensions: {}x{}", DebugName,
                     NewWidth, NewHeight);
        return false;
    }

    // 如果尺寸相同，不需要重新创建
    if (Width == NewWidth && Height == NewHeight)
    {
        return true;
    }

    // 外部ImageView模式：只更新尺寸，不重新创建资源
    // 调用者需要手动调用 SetImageView() 来更新 ImageView
    if (!bOwnsImageView)
    {
        Width  = NewWidth;
        Height = NewHeight;
        HK_LOG_INFO(ELogcat::Render,
                    "RenderTexture '{}' resized to {}x{} (external ImageView mode, call SetImageView to update)",
                    DebugName, NewWidth, NewHeight);
        return true;
    }

    // 自管理模式：释放旧资源并重新创建
    Release();

    // 更新尺寸
    Width  = NewWidth;
    Height = NewHeight;

    // 创建新资源
    if (!CreateResources())
    {
        HK_LOG_ERROR(ELogcat::Render, "Failed to resize RenderTexture '{}' to {}x{}", DebugName, NewWidth, NewHeight);
        return false;
    }

    return true;
}

void FRenderTexture::Release()
{
    FGfxDevice* Device = GetGfxDevice();
    if (!Device)
    {
        HK_LOG_ERROR(ELogcat::Render, "Cannot release RenderTexture '{}': GfxDevice is null", DebugName);
        return;
    }

    // 销毁ImageView（仅当拥有所有权时）
    if (ImageView.IsValid() && bOwnsImageView)
    {
        Device->DestroyImageView(ImageView);
    }

    // 销毁Image（仅当拥有所有权时，外部ImageView模式下不会有Image）
    if (Image.IsValid() && bOwnsImageView)
    {
        Device->DestroyImage(Image);
    }

    // 清空引用
    ImageView = FRHIImageView();
    Image     = FRHIImage();
}

void FRenderTexture::SetImageView(const FRHIImageView& InImageView)
{
    if (bOwnsImageView)
    {
        HK_LOG_WARN(ELogcat::Render,
                    "SetImageView called on RenderTexture '{}' that owns its ImageView. "
                    "This method should only be used with external ImageView mode.",
                    DebugName);
        return;
    }

    if (!InImageView.IsValid())
    {
        HK_LOG_WARN(ELogcat::Render, "SetImageView called with invalid ImageView on RenderTexture '{}'", DebugName);
    }

    ImageView = InImageView;
}

bool FRenderTexture::IsDepthFormat() const
{
    switch (Format)
    {
        case ERHIImageFormat::D16_UNorm:
        case ERHIImageFormat::D32_SFloat:
        case ERHIImageFormat::D16_UNorm_S8_UInt:
        case ERHIImageFormat::D24_UNorm_S8_UInt:
        case ERHIImageFormat::D32_SFloat_S8_UInt:
            return true;
        default:
            return false;
    }
}

bool FRenderTexture::IsStencilFormat() const
{
    switch (Format)
    {
        case ERHIImageFormat::S8_UInt:
        case ERHIImageFormat::D16_UNorm_S8_UInt:
        case ERHIImageFormat::D24_UNorm_S8_UInt:
        case ERHIImageFormat::D32_SFloat_S8_UInt:
            return true;
        default:
            return false;
    }
}

ERHIImageAspect FRenderTexture::GetImageAspect() const
{
    if (IsDepthFormat() && IsStencilFormat())
    {
        return ERHIImageAspect::Depth | ERHIImageAspect::Stencil;
    }
    else if (IsDepthFormat())
    {
        return ERHIImageAspect::Depth;
    }
    else if (IsStencilFormat())
    {
        return ERHIImageAspect::Stencil;
    }
    else
    {
        return ERHIImageAspect::Color;
    }
}

bool FRenderTexture::CreateResources()
{
    FGfxDevice* Device = GetGfxDevice();
    if (!Device)
    {
        HK_LOG_ERROR(ELogcat::Render, "Cannot create RenderTexture '{}': GfxDevice is null", DebugName);
        return false;
    }

    // 创建图像描述
    FRHIImageDesc ImageDesc;
    ImageDesc.Type          = ERHIImageType::Image2D;
    ImageDesc.Format        = Format;
    ImageDesc.Extent        = FVector3i(static_cast<Int32>(Width), static_cast<Int32>(Height), 1);
    ImageDesc.MipLevels     = 1;
    ImageDesc.ArrayLayers   = 1;
    ImageDesc.Samples       = ERHISampleCount::Sample1;
    ImageDesc.Usage         = Usage;
    ImageDesc.InitialLayout = ERHIImageLayout::Undefined;
    ImageDesc.DebugName     = DebugName;

    // 创建图像
    Image = Device->CreateImage(ImageDesc);
    if (!Image.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Render, "Failed to create RHIImage for RenderTexture '{}'", DebugName);
        return false;
    }

    // 创建图像视图描述
    FRHIImageViewDesc ViewDesc;
    ViewDesc.ViewType       = ERHIImageType::Image2D;
    ViewDesc.Format         = Format;
    ViewDesc.Aspects        = GetImageAspect();
    ViewDesc.BaseMipLevel   = 0;
    ViewDesc.LevelCount     = 1;
    ViewDesc.BaseArrayLayer = 0;
    ViewDesc.LayerCount     = 1;
    ViewDesc.DebugName      = DebugName + "_View";

    // 创建图像视图
    ImageView = Device->CreateImageView(Image, ViewDesc);
    if (!ImageView.IsValid())
    {
        HK_LOG_ERROR(ELogcat::Render, "Failed to create RHIImageView for RenderTexture '{}'", DebugName);
        Device->DestroyImage(Image);
        return false;
    }

    return true;
}
