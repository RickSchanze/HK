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

    bool Import(const TSharedPtr<FAssetMetadata>& Metadata, bool AllowIntermediateFailed = false);

    bool Import(FStringView Path);

    /**
     * 创建或者获取资产的AssetMeta, 如果不存在则创建
     * @param AssetPath
     * @return
     */
    static TSharedPtr<FAssetMetadata> GetOrCreateAssetMetadata(FStringView AssetPath);

    virtual void BeginImport() {}

    /**
     * 处理一个资产的中间资产
     */
    virtual bool ProcessAssetIntermediate()
    {
        return false;
    }

    /**
     * 处理
     * @return
     */
    virtual bool ProcessImport()
    {
        return false;
    }

    virtual void EndImport() {}

    /**
     * 校验中间文件的 Hash，如果不匹配则返回 false
     * @param IntermediatePath 中间文件路径
     * @return 如果 Hash 匹配或文件不存在则返回 true，不匹配则返回 false
     */
    bool ValidateIntermediateHash(FStringView IntermediatePath) const;

protected:
    TSharedPtr<FAssetMetadata> Metadata;
};

class FAssetMetadataFactory
{
public:
    virtual ~FAssetMetadataFactory() = default;

    /**
     * 单纯的创建一个对应的AssetMetadata
     * @return
     */
    virtual TSharedPtr<FAssetMetadata> Create() = 0;
};

class FGlobalAssetMetadataFactory : public FSingleton<FGlobalAssetMetadataFactory>
{
public:
    void StartUp() override;
    void ShutDown() override;

    void SetAssetMetadataFactory(EAssetType AssetType, TUniquePtr<FAssetMetadataFactory>&& Factory)
    {
        if (AssetType < EAssetType::Count)
        {
            if (AssetMetadataFactory[(int)AssetType])
            {
                HK_LOG_WARN(ELogcat::Asset, "Asset metadata factory for asset type {} already exists, overwrite.",
                            static_cast<int>(AssetType));
            }
            AssetMetadataFactory[(int)AssetType] = std::move(Factory);
        }
    }

    TSharedPtr<FAssetMetadata> CreateAssetMetadata(FStringView Path);

private:
    TFixedArray<TUniquePtr<FAssetMetadataFactory>, (int)EAssetType::Count> AssetMetadataFactory;
};
