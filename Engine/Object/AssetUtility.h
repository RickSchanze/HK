#pragma once

#include "AssetRegistry.h"
#include "Core/String/StringView.h"
#include "Core/Utility/SharedPtr.h"

class HObject;

/**
 * 资产工具类，提供 AssetImporter 和 AssetLoader 共用的公共方法
 */
class HK_API FAssetUtility
{
public:
    /**
     * 创建或者获取资产的 AssetMetadata，如果不存在则创建
     * @param AssetPath 资产路径
     * @return 元数据共享指针，如果创建失败则返回 nullptr
     */
    static TSharedPtr<FAssetMetadata> GetOrCreateAssetMetadata(FStringView AssetPath);

    /**
     * 校验中间文件的 Hash，如果不匹配则返回 false
     * @param Metadata 资产元数据
     * @param IntermediatePath 中间文件路径
     * @return 如果 Hash 匹配或文件不存在则返回 true，不匹配则返回 false
     */
    static bool ValidateIntermediateHash(const TSharedPtr<FAssetMetadata>& Metadata, FStringView IntermediatePath);

    /**
     * 获取中间文件路径（Texture）
     * @param Guid 资产 UUID
     * @return 中间文件路径
     */
    static FString GetTextureIntermediatePath(const FUuid& Guid);

    /**
     * 获取中间文件路径（Mesh）
     * @param Guid 资产 UUID
     * @return 中间文件路径
     */
    static FString GetMeshIntermediatePath(const FUuid& Guid);

    /**
     * 获取中间文件路径（Shader）
     * @param Guid 资产 UUID
     * @return 中间文件路径
     */
    static FString GetShaderIntermediatePath(const FUuid& Guid);

    /**
     * 通过名称查找对象（辅助函数，用于从 ObjectArray 中查找已创建的对象）
     * @param Name 对象名称
     * @return 找到的对象指针，如果未找到则返回 nullptr
     */
    template <typename T>
    static T* FindObjectByName(FName Name)
    {
        return FObjectArray::GetRef().FindObjectByName<T>(Name);
    }
};

