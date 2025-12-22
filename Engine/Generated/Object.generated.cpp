#include "Object.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Object/Object.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_HObject_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<HObject>("HObject");

    // 注册属性
    HObject::Register_HObject_Properties(Type);

}

HK_API void HObject::Z_HObject_Register::Register_HObject()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<HObject>(Register_HObject_Impl);
}

#define HObject_SERIALIZATION_CODE \
        Ar( \
        MakeNamedPair("Name", Name), \
        MakeNamedPair("ID", ID) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(HObject)

#undef HObject_SERIALIZATION_CODE

