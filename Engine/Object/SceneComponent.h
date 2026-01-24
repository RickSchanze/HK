#pragma once

#include "Component.h"
#include "Math/Transform.h"

#include "SceneComponent.generated.h"

HCLASS()
class CSceneComponent : public CComponent
{
    GENERATED_BODY(CSceneComponent)
protected:
    // 世界绝对Transform
    FTransform AbsoluteTransform;

    // 相对于Owner的Transform
    HPROPERTY()
    FTransform RelativeTransform;

    // 为true时更新RelativeTransform和AbsoluteTransform
    bool bTransformDirty;

public:
    CSceneComponent();
    ~CSceneComponent() override;
};
