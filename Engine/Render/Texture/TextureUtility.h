#pragma once

#include "Core/Container/Array.h"
#include "RHI/RHIBuffer.h"
#include "RHI/RHICommandBuffer.h"
#include "RHI/RHIImage.h"
#include "RHI/RHIImageView.h"

class HTexture;
struct FImageData;

/**
 * 纹理工具类，提供纹理上传到 GPU 的公共方法
 */
class HK_API FTextureUtility
{
public:
    /**
     * 创建 RHIImage
     * @param ImageData 图像数据
     * @param Format 图像格式
     * @return RHIImage，如果创建失败则返回无效句柄
     */
    static FRHIImage CreateRHIImage(const FImageData& ImageData, ERHIImageFormat Format);

    /**
     * 创建 RHIImageView
     * @param Image 图像句柄
     * @param Format 图像格式
     * @return RHIImageView，如果创建失败则返回无效句柄
     */
    static FRHIImageView CreateRHIImageView(const FRHIImage& Image, ERHIImageFormat Format);

    /**
     * 上传纹理数据到 GPU
     * @param ImageData 图像数据
     * @param Image 目标图像句柄
     * @param ImageFormat 图像格式
     * @param OutStagingBuffer 输出的 staging buffer（用于后续清理）
     * @param OutCommandBuffer 输出的 command buffer（用于后续清理）
     * @return 如果上传成功返回 true，否则返回 false
     */
    static bool UploadTextureToGPU(const FImageData& ImageData, const FRHIImage& Image, ERHIImageFormat ImageFormat,
                                    FRHIBuffer& OutStagingBuffer, FRHICommandBuffer& OutCommandBuffer);

    /**
     * 从 Intermediate 数据创建并上传纹理到 GPU
     * @param Intermediate 中间数据
     * @param OutImage 输出的图像句柄
     * @param OutImageView 输出的图像视图句柄
     * @param OutStagingBuffer 输出的 staging buffer（用于后续清理）
     * @param OutCommandBuffer 输出的 command buffer（用于后续清理）
     * @return 如果创建成功返回 true，否则返回 false
     */
    static bool CreateAndUploadTextureFromIntermediate(const struct FTextureIntermediate& Intermediate,
                                                        FRHIImage& OutImage, FRHIImageView& OutImageView,
                                                        FRHIBuffer& OutStagingBuffer,
                                                        FRHICommandBuffer& OutCommandBuffer);

    /**
     * 设置 HTexture 对象的 RHI 资源
     * @param Texture 纹理对象
     * @param Image 图像句柄
     * @param ImageView 图像视图句柄
     * @param Width 宽度
     * @param Height 高度
     * @param Format 格式
     */
    static void SetTextureRHIResources(HTexture* Texture, const FRHIImage& Image, const FRHIImageView& ImageView,
                                       Int32 Width, Int32 Height, ERHIImageFormat Format);
};

