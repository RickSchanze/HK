#pragma once
#include "Core/Reflection/Reflection.h"

#include "Color.generated.h"

HSTRUCT()
struct HK_API FColor
{
    GENERATED_BODY(FColor)
public:
    HPROPERTY()
    Float R{};

    HPROPERTY()
    Float G{};

    HPROPERTY()
    Float B{};

    HPROPERTY()
    Float A{0};
};
