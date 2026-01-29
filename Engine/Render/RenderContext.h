#pragma once
#include "Core/Singleton/Singleton.h"
#include "RHI/RHICommandPool.h"
#include "RHI/RHISync.h"
#include "RenderOptions.h"

class FRenderContext : public TSingleton<FRenderContext>
{
public:
    void StartUp() override;
    void ShutDown() override;

    /**
     * 获取全局上传命令池（用于资产导入时的 GPU 上传操作）
     */
    [[nodiscard]] FRHICommandPool GetUploadCommandPool() const
    {
        return UploadCommandPool;
    }

    static inline void Render()
    {
        GetRef().RenderFrame();
    }

    /**
     * 渲染并呈现一帧
     */
    void RenderFrame();

private:
    FRHICommandPool UploadCommandPool;

    TFixedArray<FRHIFence, HK_RENDER_INIT_FRAME_IN_FLIGHT> InFlightFences;
};
