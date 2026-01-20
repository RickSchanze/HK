#pragma once
#include "Core/Container/Buffer.h"
#include "Core/Utility/SharedPtr.h"
#include "Object/Asset.h"
#include "Object/ObjectPtr.h"

#include "Material.generated.h"

class HTexture;
class HShader;
struct FSharedMaterial;

struct FMaterialParameter
{
    FName  Name;
    UInt32 Offset;
    UInt32 Size;

    bool IsValid() const
    {
        return Size != 0;
    }

    FMaterialParameter(const FName InName, const UInt32 InOffset, const UInt32 InSize)
        : Name(InName), Offset(InOffset), Size(InSize)
    {
    }
};

struct FMaterialParameterBlock
{
private:
    // Offset从小到大排列
    TArray<FMaterialParameter> Parameters;
    UInt32                     BlockSize;

public:
    explicit FMaterialParameterBlock(const HShader* InShader);

    bool IsValid() const
    {
        return BlockSize != 0;
    }

    FMaterialParameter GetParameter(const FName& InName) const
    {
        for (const auto& Parameter : Parameters)
        {
            if (Parameter.Name == InName)
            {
                return Parameter;
            }
        }
        return {Names::None, 0, 0};
    }

    UInt32 GetBlockSize() const
    {
        return BlockSize;
    }
};

HCLASS()
class HMaterial : public HAsset
{
    GENERATED_BODY(HMaterial)
public:
    HMaterial();
    ~HMaterial() override;

    /**
     * 绑定一个纹理参数
     * @param InName
     * @param InTexture
     */
    void BindTexture(const FName& InName, HTexture* InTexture);

    /**
     * 绑定一个模型矩阵的索引
     * @param InModelIndex
     */
    void BindModelMatrixIndex(const UInt32 InModelIndex);

protected:
    void SetParameterUpdateDirty(bool InDirty)
    {
        bParameterUpdateDirty = InDirty;
    }

    bool bParameterUpdateDirty = true;

private:
    HPROPERTY()
    TObjectPtr<class HShader> Shader;

    TSharedPtr<FSharedMaterial> SharedMaterial;

    // 参数Buffer, 使用PushConstant传递给GPU
    FBuffer ParameterBuffer;
    // 参数块的信息
    FMaterialParameterBlock ParameterBlock;
};
