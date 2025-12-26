#pragma once
#include "Asset.h"
#include "Core/Reflection/Reflection.h"
#include "Core/String/StringView.h"
#include "Core/Utility/Uuid.h"

#include "AssetRegistry.generated.h"
#include "Core/Container/LruCache.h"
#include "Core/Utility/SharedPtr.h"

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
struct HK_API FAssetMetaData
{
    GENERATED_BODY(FAssetMetaData)
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
};

class FAssetRegistry : public FSingleton<FAssetRegistry>
{
public:
    FAssetRegistry() : CachedMetadata(512) {}

    FAssetMetaData& LoadAssetMetadata(const FString& Path);
    FAssetMetaData& LoadAssetMetadata(FUuid Uuid);

    // 保存元数据（保存后会自动更新缓存）
    bool SaveAssetMetadata(const FAssetMetaData& Metadata);

    // 通过路径保存（会查找缓存中的 Metadata，如果不存在则返回 false）
    bool SaveAssetMetadata(const FString& Path);

    // 通过 UUID 保存（会查找缓存中的 Metadata，如果不存在则返回 false）
    bool SaveAssetMetadata(const FUuid& Uuid);

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
    TMap<FUuid, FString> UuidToPath;
    TMap<FString, FUuid> PathToUuid;
    TLruCache<FUuid, FAssetMetaData> CachedMetadata;
};
