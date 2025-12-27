#include "Texture.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Texture/Texture.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_HTexture_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<HTexture>("HTexture");

    // 注册父类: HAsset
    Type->RegisterParent(FTypeManager::TypeOf<HAsset>());

}

void HTexture::Z_HTexture_Register::Register_HTexture()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<HTexture>(Register_HTexture_Impl);
}

#define HTexture_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(HTexture)

#undef HTexture_SERIALIZATION_CODE

