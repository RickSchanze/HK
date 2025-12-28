#pragma once
#include "Core/Singleton/Singleton.h"
#include "RHI/RHICommandPool.h"

class FRenderContext : public FSingleton<FRenderContext>
{
public:
    void StartUp() override;
    void ShutDown() override;

    /**
     * 获取全局上传命令池（用于资产导入时的 GPU 上传操作）
     */
    FRHICommandPool GetUploadCommandPool() const
    {
        return UploadCommandPool;
    }

private:
    FRHICommandPool UploadCommandPool;
};
