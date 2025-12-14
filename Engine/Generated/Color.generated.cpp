#include "Color.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Math/Color.h"


static void Register_FColor_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FColor>("Color");

    // 注册属性: R
    Type->RegisterProperty(&FColor::R, "R");

    // 注册属性: G
    Type->RegisterProperty(&FColor::G, "G");

    // 注册属性: B
    Type->RegisterProperty(&FColor::B, "B");

    // 注册属性: A
    Type->RegisterProperty(&FColor::A, "A");

}

void FColor::Z_Color_Register::Register_FColor()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FColor>(Register_FColor_Impl);
}


static void Z_Register_MyEnum_Impl()
{
    // 注册枚举类型
    FTypeMutable Type = FTypeManager::Register<MyEnum>("MyEnum");

    // 注册枚举成员: A
    Type->RegisterEnumMember(MyEnum::A, "A");

    // 注册枚举成员: B
    Type->RegisterEnumMember(MyEnum::B, "B");

    // 注册枚举成员: C
    Type->RegisterEnumMember(MyEnum::C, "C");

    // 注册枚举成员: D
    Type->RegisterEnumMember(MyEnum::D, "D");

}

void Z_Register_MyEnum()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<MyEnum>(Z_Register_MyEnum_Impl);
}
