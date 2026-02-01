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

    virtual void EndImport([[maybe_unused]] bool Success) {}

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

class FGlobalAssetMetadataFactory : public TSingleton<FGlobalAssetMetadataFactory>
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
