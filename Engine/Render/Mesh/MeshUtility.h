#pragma once

#include "Core/Container/Array.h"
#include "RHI/RHIBuffer.h"
#include "RHI/RHICommandBuffer.h"

struct FSubMesh;
struct FMeshIntermediate;

/**
 * Mesh 工具类，提供 Mesh 上传到 GPU 的公共方法
 */
class HK_API FMeshUtility
{
public:
    /**
     * 从 Intermediate 数据创建并上传 Mesh 到 GPU
     * @param Intermediate 中间数据
     * @param OutSubMeshes 输出的 SubMesh 数组
     * @param OutStagingBuffers 输出的 staging buffer 数组（用于后续清理）
     * @param OutCommandBuffers 输出的 command buffer 数组（用于后续清理）
     * @return 如果创建成功返回 true，否则返回 false
     */
    static bool CreateAndUploadMeshFromIntermediate(const FMeshIntermediate& Intermediate,
                                                     TArray<FSubMesh>& OutSubMeshes,
                                                     TArray<FRHIBuffer>& OutStagingBuffers,
                                                     TArray<FRHICommandBuffer>& OutCommandBuffers);
};

