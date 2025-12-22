#include "Asset.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Object/Asset.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_HAsset_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<HAsset>("HAsset");

}

HK_API void HAsset::Z_HAsset_Register::Register_HAsset()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<HAsset>(Register_HAsset_Impl);
}

#define HAsset_SERIALIZATION_CODE \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(HAsset)

#undef HAsset_SERIALIZATION_CODE

