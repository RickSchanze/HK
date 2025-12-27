#pragma once
#include "AssetRegistry.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/StringView.h"
#include "Core/Utility/SharedPtr.h"
#include "Core/Utility/UniquePtr.h"
#include "RHI/RHICommandPool.h"

// 前向声明
class FGfxDevice;

HENUM()
enum class EAssetImportOptions
{
    None        = 9,
    ForceImport = 1 << 0, // 当已经存在时, 强制导入
};
HK_ENABLE_BITMASK_OPERATORS(EAssetImportOptions)

class FAssetImporter
{
public:
    virtual ~FAssetImporter() = default;

    virtual bool Import(FStringView Path, EAssetFileType FileType, EAssetImportOptions Options) = 0;

    /**
     * 获取或创建导入设置
     * @param Metadata 资源元数据
     * @return 返回对应类型的导入设置，如果类型不匹配则返回 nullptr
     */
    virtual TSharedPtr<FAssetImportSetting> GetOrCreateImportSetting(FAssetMetaData& Metadata) = 0;
};

class FGlobalAssetImporter : public FSingleton<FGlobalAssetImporter>
{
public:
    void StartUp() override;
    void ShutDown() override;

    void SetTextureImporter(TUniquePtr<FAssetImporter>&& InTextureImporter)
    {
        TextureImporter = std::move(InTextureImporter);
    }

    void SetMeshImporter(TUniquePtr<FAssetImporter>&& InMeshImporter)
    {
        MeshImporter = std::move(InMeshImporter);
    }

    void SetShaderImporter(TUniquePtr<FAssetImporter>&& InShaderImporter)
    {
        ShaderImporter = std::move(InShaderImporter);
    }

    /**
     * 自动判断路径对应的资源类型并导入
     */
    void Import(FStringView);

    /**
     * 获取用于资源上传的共享 CommandPool
     * 所有 Importer 都应该使用这个共享的 CommandPool
     */
    FRHICommandPool GetUploadCommandPool() const
    {
        return UploadCommandPool;
    }

private:
    TUniquePtr<FAssetImporter> TextureImporter;
    TUniquePtr<FAssetImporter> MeshImporter;
    TUniquePtr<FAssetImporter> ShaderImporter;

    // 共享的 CommandPool，用于所有资源上传操作
    FRHICommandPool UploadCommandPool;

    // GfxDevice 销毁回调的 Handle，用于在 ShutDown 时清理
    TEvent<FGfxDevice*>::Handle PreDestroyCallbackHandle = 0;
};
