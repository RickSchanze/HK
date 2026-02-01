#include "RenderTarget.h"

#include "Core/Logging/Logger.h"
#include "Texture/RenderTexture.h"

FRenderTarget::FRenderTarget(const TArray<FRenderTargetColorAttachment>& InColorAttachments, FStringView InDebugName)
    : ColorAttachments(InColorAttachments), DebugName(InDebugName)
{
    AutoSetRenderArea();
}

FRenderTarget::FRenderTarget(const TArray<FRenderTargetColorAttachment>& InColorAttachments,
                             const FRenderTargetDepthStencilAttachment&  InDepthStencilAttachment,
                             FStringView                                 InDebugName)
    : ColorAttachments(InColorAttachments), DepthStencilAttachment(InDepthStencilAttachment), bHasDepthStencil(true),
      DebugName(InDebugName)
{
    AutoSetRenderArea();
}

void FRenderTarget::AddColorAttachment(const FRenderTargetColorAttachment& Attachment)
{
    if (!Attachment.RenderTexture)
    {
        HK_LOG_WARN(ELogcat::Render, "Trying to add null RenderTexture to RenderTarget '{}'", DebugName);
        return;
    }

    if (!Attachment.RenderTexture->IsValid())
    {
        HK_LOG_WARN(ELogcat::Render, "Trying to add invalid RenderTexture to RenderTarget '{}'", DebugName);
        return;
    }

    ColorAttachments.Add(Attachment);

    // 如果渲染区域未设置，自动设置为第一个附件的尺寸
    if (ColorAttachments.Size() == 1 && RenderArea.Width == 0 && RenderArea.Height == 0)
    {
        AutoSetRenderArea();
    }
}

void FRenderTarget::SetDepthStencilAttachment(const FRenderTargetDepthStencilAttachment& Attachment)
{
    if (!Attachment.RenderTexture)
    {
        HK_LOG_WARN(ELogcat::Render, "Trying to set null RenderTexture as depth/stencil for RenderTarget '{}'",
                    DebugName);
        return;
    }

    if (!Attachment.RenderTexture->IsValid())
    {
        HK_LOG_WARN(ELogcat::Render, "Trying to set invalid RenderTexture as depth/stencil for RenderTarget '{}'",
                    DebugName);
        return;
    }

    if (!Attachment.RenderTexture->IsDepthFormat())
    {
        HK_LOG_WARN(ELogcat::Render, "Trying to set non-depth RenderTexture as depth/stencil for RenderTarget '{}'",
                    DebugName);
        return;
    }

    DepthStencilAttachment = Attachment;
    bHasDepthStencil       = true;

    // 如果渲染区域未设置且没有颜色附件，自动设置为深度附件的尺寸
    if (ColorAttachments.IsEmpty() && RenderArea.Width == 0 && RenderArea.Height == 0)
    {
        AutoSetRenderArea();
    }
}

void FRenderTarget::ClearAttachments()
{
    ColorAttachments.Clear();
    bHasDepthStencil = false;
    RenderArea       = FRect2Di(0, 0, 0, 0);
}

void FRenderTarget::AutoSetRenderArea()
{
    // 优先使用颜色附件的尺寸
    if (!ColorAttachments.IsEmpty() && ColorAttachments[0].RenderTexture)
    {
        const FVector2i Size = ColorAttachments[0].RenderTexture->GetSize();
        RenderArea           = FRect2Di(0, 0, static_cast<UInt32>(Size.X), static_cast<UInt32>(Size.Y));
    }
    // 否则使用深度附件的尺寸
    else if (bHasDepthStencil && DepthStencilAttachment.RenderTexture)
    {
        const FVector2i Size = DepthStencilAttachment.RenderTexture->GetSize();
        RenderArea           = FRect2Di(0, 0, static_cast<UInt32>(Size.X), static_cast<UInt32>(Size.Y));
    }
}
