#include "AssetLoader.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Object/AssetLoader.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FAssetLoader_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FAssetLoader>("AssetLoader");

    // 注册类型属性: Abstract
    Type->RegisterAttribute(FName("Abstract"), "");

}

HK_API void FAssetLoader::Z_AssetLoader_Register::Register_FAssetLoader()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FAssetLoader>(Register_FAssetLoader_Impl);
}

#define FAssetLoader_SERIALIZATION_CODE \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(FAssetLoader)

#undef FAssetLoader_SERIALIZATION_CODE

