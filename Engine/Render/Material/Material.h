#pragma once
#include "Core/Utility/SharedPtr.h"
#include "Object/Asset.h"
#include "Object/ObjectPtr.h"

#include "Material.generated.h"

struct FSharedMaterial;
HCLASS()
class HMaterial : public HAsset
{
    GENERATED_BODY(HMaterial)
public:
    HMaterial();
    ~HMaterial() override;

private:
    HPROPERTY()
    TObjectPtr<class HShader> Shader;

    TSharedPtr<FSharedMaterial> SharedMaterial;
};
