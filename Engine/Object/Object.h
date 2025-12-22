#pragma once
#include "Core/Reflection/Reflection.h"

#include "Object.generated.h"

typedef Int64 FObjectID;
constexpr inline FObjectID INVALID_OBJECT_ID = 0LL;

HCLASS()
class HK_API HObject
{
    GENERATED_BODY(HObject)
public:
    HObject() {}
    virtual ~HObject() = default;

    FObjectID GetID() const { return ID; }
    FName GetName() const { return Name; }

protected:
    HPROPERTY()
    FName Name;

private:
    HPROPERTY()
    FObjectID ID = INVALID_OBJECT_ID;
};
