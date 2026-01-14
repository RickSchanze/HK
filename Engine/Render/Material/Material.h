#pragma once
#include "Object/Asset.h"
#include "Object/ObjectPtr.h"

HCLASS()
class HMaterial : public HAsset
{
public:


private:
    HPROPERTY()
    TObjectPtr<class HShader> Shader;
};
