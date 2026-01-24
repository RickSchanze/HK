#pragma once

#include "Object.h"
#include "ObjectPtr.h"

#include "Component.generated.h"

HCLASS()
class CComponent : public HObject
{
    GENERATED_BODY(CComponent)
protected:
    HPROPERTY()
    bool bActive = true;

    HPROPERTY()
    TObjectPtr<class AActor> Owner;

public:
    CComponent();
    ~CComponent() override;

    void SetActive(const bool bActive)
    {
        if (this->bActive != bActive)
        {
            this->bActive = bActive;
            if (bActive)
            {
                OnActive();
            }
            else
            {
                OnInactive();
            }
        }
    }

    [[nodiscard]] bool IsActive() const
    {
        return bActive;
    }

    virtual void OnActive() {}
    virtual void OnInactive() {}

    const AActor* GetOwner() const
    {
        return Owner.Get();
    }

    AActor* GetOwner()
    {
        return Owner.Get();
    }
};
