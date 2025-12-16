#pragma once
#include "Core/Reflection/Reflection.h"

#include "TestStruct.generated.h"

// 父类 struct
HSTRUCT()
struct HK_API FBaseStruct
{
    GENERATED_BODY(FBaseStruct)
public:
    HPROPERTY()
    Int32 BaseValue{0};
};

// 子类 struct 继承父类
HSTRUCT()
struct HK_API FDerivedStruct : FBaseStruct
{
    GENERATED_BODY(FDerivedStruct)
public:
    HPROPERTY()
    Float DerivedValue{1.0f};
};
