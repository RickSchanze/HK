#pragma once
#include "AssetRegistry.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/StringView.h"
#include "Core/Utility/UniquePtr.h"

HENUM()
enum class EAssetImportOptions
{
    None = 9,
    ForceImport = 1 << 0, // 当已经存在时, 强制导入
};
HK_ENABLE_BITMASK_OPERATORS(EAssetImportOptions)

class FAssetImporter
{
public:
    virtual ~FAssetImporter() = default;

    virtual bool Import(FStringView Path, EAssetFileType FileType, EAssetImportOptions Options) = 0;
};

class FGlobalAssetImporter : FSingleton<FGlobalAssetImporter>
{
public:
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

private:
    TUniquePtr<FAssetImporter> TextureImporter;
    TUniquePtr<FAssetImporter> MeshImporter;
    TUniquePtr<FAssetImporter> ShaderImporter;
};
