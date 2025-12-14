#include "TestClass.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Math/TestClass.h"


static void Register_FBaseClass_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FBaseClass>("BaseClass");

    // 注册属性: BaseValue
    Type->RegisterProperty(&FBaseClass::BaseValue, "BaseValue");

}

void FBaseClass::Z_BaseClass_Register::Register_FBaseClass()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FBaseClass>(Register_FBaseClass_Impl);
}

#define FBaseClass_SERIALIZATION_CODE \
        Ar( \
        MakeNamedPair("BaseValue", BaseValue) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(FBaseClass)

#undef FBaseClass_SERIALIZATION_CODE


static void Register_FDerivedClass_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FDerivedClass>("DerivedClass");

    // 注册父类: FBaseClass
    Type->RegisterParent(FTypeManager::TypeOf<FBaseClass>());

    // 注册属性: DerivedValue
    Type->RegisterProperty(&FDerivedClass::DerivedValue, "DerivedValue");

}

void FDerivedClass::Z_DerivedClass_Register::Register_FDerivedClass()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FDerivedClass>(Register_FDerivedClass_Impl);
}

#define FDerivedClass_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        Ar( \
        MakeNamedPair("DerivedValue", DerivedValue) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(FDerivedClass)

#undef FDerivedClass_SERIALIZATION_CODE


static void Register_FSimpleClass_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FSimpleClass>("SimpleClass");

    // 注册属性: Value
    Type->RegisterProperty(&FSimpleClass::Value, "Value");

}

void FSimpleClass::Z_SimpleClass_Register::Register_FSimpleClass()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FSimpleClass>(Register_FSimpleClass_Impl);
}

#define FSimpleClass_SERIALIZATION_CODE \
        Ar( \
        MakeNamedPair("Value", Value) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(FSimpleClass)

#undef FSimpleClass_SERIALIZATION_CODE

