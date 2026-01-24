#pragma once
#include "Math/Transform.h"
#include "Object.h"
#include "ObjectPtr.h"

#include "Actor.generated.h"

HCLASS()
class AActor : public HObject
{
    GENERATED_BODY(AActor)
protected:
    HPROPERTY()
    TArray<TObjectPtr<class CComponent>> Components;

    HPROPERTY()
    FTransform RelativeTransform;

    FTransform AbsoluteTransform;

    bool bTransformDirty = true;

public:
    AActor();
    virtual ~AActor() override;
};