#include "TestStruct.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Math/TestStruct.h"


static void Register_FBaseStruct_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FBaseStruct>("BaseStruct");

    // 注册属性: BaseValue
    Type->RegisterProperty(&FBaseStruct::BaseValue, "BaseValue");

}

void FBaseStruct::Z_BaseStruct_Register::Register_FBaseStruct()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FBaseStruct>(Register_FBaseStruct_Impl);
}


static void Register_FDerivedStruct_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FDerivedStruct>("DerivedStruct");

    // 注册父类: FBaseStruct
    Type->RegisterParent(FTypeManager::TypeOf<FBaseStruct>());

    // 注册属性: DerivedValue
    Type->RegisterProperty(&FDerivedStruct::DerivedValue, "DerivedValue");

}

void FDerivedStruct::Z_DerivedStruct_Register::Register_FDerivedStruct()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FDerivedStruct>(Register_FDerivedStruct_Impl);
}

