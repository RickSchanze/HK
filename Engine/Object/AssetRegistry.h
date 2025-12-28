#pragma once
#include "Asset.h"
#include "Core/Container/LruCache.h"
#include "Core/Reflection/Reflection.h"
#include "Core/String/StringView.h"
#include "Core/Utility/SharedPtr.h"
#include "Core/Utility/Uuid.h"

#include "AssetRegistry.generated.h"

HENUM()
enum class EAssetFileType
{
    Unknown,

    // 图像格式
    PNG,
    JPG,
    JPEG,
    BMP,
    TGA,
    HDR,
    EXR,
    DDS,
    KTX,
    KTX2,

    // 3D 模型格式
    FBX,
    OBJ,
    GLTF,
    GLB,
    DAE,
    BLEND,
    X3D,

    // 音频格式
    WAV,
    OGG,
    MP3,
    FLAC,

    // 视频格式
    MP4,
    AVI,
    MOV,

    // 着色器格式
    HLSL,
    GLSL,
    SLANG,
    SPIRV,

    // 其他
    JSON,
    XML,
    TXT,

    Count,
};

HCLASS()
class HK_API FAssetImportSetting
{
    GENERATED_BODY(FAssetImportSetting)
public:
    virtual ~FAssetImportSetting() = default;
};

HSTRUCT()
struct HK_API FAssetMetadata
{
    GENERATED_BODY(FAssetMetadata)
public:
    HPROPERTY()
    FUuid Uuid;

    HPROPERTY()
    FString Path;

    HPROPERTY()
    EAssetType AssetType = EAssetType::Count;

    HPROPERTY()
    EAssetFileType FileType = EAssetFileType::Unknown;

    HPROPERTY()
    TSharedPtr<FAssetImportSetting> ImportSetting;

    HPROPERTY()
    UInt64 IntermediateHash = 0; // 中间文件的 Hash 值，用于快速校验
};

class FAssetRegistry : public FSingleton<FAssetRegistry>
{
public:
    FAssetRegistry() : CachedMetadata(512) {}

    /**
     * 加载资产元数据
     * @param Path 资产路径
     * @return 元数据的共享指针，如果加载失败则返回 nullptr
     */
    TSharedPtr<FAssetMetadata> LoadAssetMetadata(FStringView Path);

    /**
     * 加载资产元数据
     * @param Uuid 资产UUID
     * @return 元数据的共享指针，如果加载失败则返回 nullptr
     */
    TSharedPtr<FAssetMetadata> LoadAssetMetadata(const FUuid& Uuid);

    /**
     * 创建资产元数据
     * @param Path 资产路径
     * @return 元数据的共享指针，如果已存在则警告并返回已存在的元数据，如果创建失败则返回 nullptr
     */
    TSharedPtr<FAssetMetadata> CreateAssetMetadata(FStringView Path);

    /**
     * 保存资产元数据
     * @param Metadata 元数据共享指针
     * @return 保存成功返回 true，失败返回 false。如果元数据已存在则更新
     */
    bool SaveAssetMetadata(TSharedPtr<FAssetMetadata>& Metadata);

    /**
     * 保存资产元数据（通过路径）
     * @param Path 资产路径
     * @return 保存成功返回 true，如果元数据不存在则返回 false 并记录错误
     */
    bool SaveAssetMetadata(FStringView Path);

    /**
     * 保存资产元数据（通过UUID）
     * @param Uuid 资产UUID
     * @return 保存成功返回 true，如果元数据不存在则返回 false 并记录错误
     */
    bool SaveAssetMetadata(const FUuid& Uuid);

    /**
     * 检查资产元数据是否存在
     * @param Path 资产路径
     * @return 存在返回 true，不存在返回 false
     */
    bool IsAssetMetadataExist(FStringView Path) const;

    /**
     * 检查资产元数据是否存在
     * @param Uuid 资产UUID
     * @return 存在返回 true，不存在返回 false
     */
    bool IsAssetMetadataExist(const FUuid& Uuid) const;

    /**
     * 根据文件扩展名推断文件类型
     * @param Path 文件路径
     * @return 文件类型，如果无法推断则返回 Unknown
     */
    static EAssetFileType InferFileTypeFromPath(const FStringView& Path);

    /**
     * 根据文件扩展名推断文件类型
     * @param Extension 文件扩展名（带或不带点，如 ".png" 或 "png"）
     * @return 文件类型，如果无法推断则返回 Unknown
     */
    static EAssetFileType InferFileTypeFromExtension(const FStringView& Extension);

private:
    TMap<FUuid, FString>                        UuidToPath;
    TMap<FString, FUuid>                        PathToUuid;
    TLruCache<FUuid, TSharedPtr<FAssetMetadata>> CachedMetadata;
};
