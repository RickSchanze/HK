#pragma once
#include "AssetRegistry.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/StringView.h"
#include "Core/Utility/UniquePtr.h"

class FAssetImporter
{
public:
    virtual ~FAssetImporter() = default;

    virtual bool Import(FStringView Path, EAssetFileType FileType) = 0;
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

    /**
     * 自动判断路径对应的资源类型并导入
     */
    void Import(FStringView);

private:
    TUniquePtr<FAssetImporter> TextureImporter;
    TUniquePtr<FAssetImporter> MeshImporter;
};
