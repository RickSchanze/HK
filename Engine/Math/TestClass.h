#pragma once
#include "Core/Reflection/Reflection.h"

#include "TestClass.generated.h"

// 父类 class
HCLASS()
class FBaseClass
{
    GENERATED_BODY(FBaseClass)
public:
    virtual ~FBaseClass() = default;

    HPROPERTY()
    Int32 BaseValue{0};
};

// 子类 class 继承父类
HCLASS()
class FDerivedClass : public FBaseClass
{
    GENERATED_BODY(FDerivedClass)
public:
    HPROPERTY()
    Float DerivedValue{1.0f};
};

// 没有父类的 class
HCLASS()
class FSimpleClass
{
    GENERATED_BODY(FSimpleClass)
public:
    virtual ~FSimpleClass() = default;

    HPROPERTY()
    Int32 Value{42};
};
