#pragma once
#include "Core/Reflection/Reflection.h"

#include "Color.generated.h"

HENUM()
enum class MyEnum
{
    A,
    B,
    C,
    D
};

HSTRUCT()
struct FColor
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
