#pragma once
#include "Core/Reflection/Reflection.h"

#include "TestStruct.generated.h"

// 父类 struct
HSTRUCT()
struct FBaseStruct
{
    GENERATED_BODY(FBaseStruct)
public:
    HPROPERTY()
    Int32 BaseValue{0};
};

// 子类 struct 继承父类
HSTRUCT()
struct FDerivedStruct : FBaseStruct
{
    GENERATED_BODY(FDerivedStruct)
public:
    HPROPERTY()
    Float DerivedValue{1.0f};
};
