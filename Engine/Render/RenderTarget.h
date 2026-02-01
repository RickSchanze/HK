#pragma once

#include "Core/Container/Array.h"
#include "Core/String/String.h"
#include "Core/Utility/Macros.h"
#include "Math/Rect2D.h"
#include "Math/Vector.h"
#include "RHI/RHIImage.h"

// 前向声明
class FRenderTexture;

/**
 * 附件加载操作（Load Operation）
 * 定义渲染开始时如何处理附件的现有内容
 */
enum class ERenderTargetLoadOp : UInt32
{
    Load     = 0, // 加载现有内容
    Clear    = 1, // 清除为指定的清除值
    DontCare = 2, // 不关心现有内容（可能带来性能优化）
};

/**
 * 附件存储操作（Store Operation）
 * 定义渲染结束时如何处理附件的内容
 */
enum class ERenderTargetStoreOp : UInt32
{
    Store    = 0, // 存储渲染结果
    DontCare = 1, // 不关心渲染结果（可能带来性能优化，适用于临时附件）
};

/**
 * 颜色附件信息
 */
struct FRenderTargetColorAttachment
{
    FRenderTexture*      RenderTexture = nullptr;                                 // 渲染纹理指针
    ERenderTargetLoadOp  LoadOp        = ERenderTargetLoadOp::Clear;              // 加载操作
    ERenderTargetStoreOp StoreOp       = ERenderTargetStoreOp::Store;             // 存储操作
    FVector4f            ClearColor    = FVector4f(0.0f, 0.0f, 0.0f, 1.0f);       // 清除颜色（LoadOp为Clear时使用）
    ERHIImageLayout      Layout        = ERHIImageLayout::ColorAttachmentOptimal; // 最终布局
};

/**
 * 深度/模板附件信息
 */
struct FRenderTargetDepthStencilAttachment
{
    FRenderTexture*      RenderTexture  = nullptr;                                        // 渲染纹理指针
    ERenderTargetLoadOp  DepthLoadOp    = ERenderTargetLoadOp::Clear;                     // 深度加载操作
    ERenderTargetStoreOp DepthStoreOp   = ERenderTargetStoreOp::Store;                    // 深度存储操作
    ERenderTargetLoadOp  StencilLoadOp  = ERenderTargetLoadOp::DontCare;                  // 模板加载操作
    ERenderTargetStoreOp StencilStoreOp = ERenderTargetStoreOp::DontCare;                 // 模板存储操作
    float                ClearDepth     = 1.0f;                                           // 清除深度值
    UInt32               ClearStencil   = 0;                                              // 清除模板值
    ERHIImageLayout      Layout         = ERHIImageLayout::DepthStencilAttachmentOptimal; // 最终布局
};

/**
 * RenderTarget - 渲染目标类
 * 用于Dynamic Rendering，可以包含多个颜色附件和一个深度/模板附件
 */
class HK_API FRenderTarget
{
public:
    /**
     * 默认构造函数
     */
    FRenderTarget() = default;

    /**
     * 构造函数 - 创建带颜色附件的RenderTarget
     * @param InColorAttachments 颜色附件数组
     * @param InDebugName 调试名称
     */
    explicit FRenderTarget(const TArray<FRenderTargetColorAttachment>& InColorAttachments,
                           FStringView                                 InDebugName = "RenderTarget");

    /**
     * 构造函数 - 创建带颜色和深度附件的RenderTarget
     * @param InColorAttachments 颜色附件数组
     * @param InDepthStencilAttachment 深度/模板附件
     * @param InDebugName 调试名称
     */
    FRenderTarget(const TArray<FRenderTargetColorAttachment>& InColorAttachments,
                  const FRenderTargetDepthStencilAttachment&  InDepthStencilAttachment,
                  FStringView                                 InDebugName = "RenderTarget");

    ~FRenderTarget() = default;

    // 允许拷贝和移动
    FRenderTarget(const FRenderTarget&)            = default;
    FRenderTarget& operator=(const FRenderTarget&) = default;
    FRenderTarget(FRenderTarget&&)                 = default;
    FRenderTarget& operator=(FRenderTarget&&)      = default;

    /**
     * 添加颜色附件
     * @param Attachment 颜色附件
     */
    void AddColorAttachment(const FRenderTargetColorAttachment& Attachment);

    /**
     * 设置深度/模板附件
     * @param Attachment 深度/模板附件
     */
    void SetDepthStencilAttachment(const FRenderTargetDepthStencilAttachment& Attachment);

    /**
     * 清除所有附件
     */
    void ClearAttachments();

    /**
     * 设置渲染区域
     * @param InRenderArea 渲染区域
     */
    void SetRenderArea(const FRect2Di& InRenderArea)
    {
        RenderArea = InRenderArea;
    }

    /**
     * 自动设置渲染区域为第一个附件的尺寸
     */
    void AutoSetRenderArea();

    /**
     * 检查RenderTarget是否有效
     * @return 如果至少有一个附件则返回true
     */
    bool IsValid() const
    {
        return !ColorAttachments.IsEmpty() || bHasDepthStencil;
    }

    // Getter方法
    const TArray<FRenderTargetColorAttachment>& GetColorAttachments() const
    {
        return ColorAttachments;
    }

    const FRenderTargetDepthStencilAttachment& GetDepthStencilAttachment() const
    {
        return DepthStencilAttachment;
    }

    bool HasDepthStencil() const
    {
        return bHasDepthStencil;
    }

    const FRect2Di& GetRenderArea() const
    {
        return RenderArea;
    }

    FStringView GetDebugName() const
    {
        return DebugName;
    }

    UInt32 GetColorAttachmentCount() const
    {
        return static_cast<UInt32>(ColorAttachments.Size());
    }

private:
    TArray<FRenderTargetColorAttachment> ColorAttachments;
    FRenderTargetDepthStencilAttachment  DepthStencilAttachment;
    bool                                 bHasDepthStencil = false;
    FRect2Di                             RenderArea       = FRect2Di(0, 0, 0, 0);
    FString                              DebugName        = "RenderTarget";
};
