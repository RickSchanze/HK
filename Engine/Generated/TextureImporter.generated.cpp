#include "TextureImporter.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Render/Texture/TextureImporter.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FTextureImportSetting_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FTextureImportSetting>("TextureImportSetting");

    // 注册父类: FAssetImportSetting
    Type->RegisterParent(FTypeManager::TypeOf<FAssetImportSetting>());

    // 注册属性
    FTextureImportSetting::Register_FTextureImportSetting_Properties(Type);

}

void FTextureImportSetting::Z_TextureImportSetting_Register::Register_FTextureImportSetting()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FTextureImportSetting>(Register_FTextureImportSetting_Impl);
}

#define FTextureImportSetting_SERIALIZATION_CODE \
        Super::Serialize(Ar); \
        Ar( \
        MakeNamedPair("GPUFormat", GPUFormat) \
        ); \


HK_DEFINE_CLASS_SERIALIZATION(FTextureImportSetting)

#undef FTextureImportSetting_SERIALIZATION_CODE


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FTextureIntermediate_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FTextureIntermediate>("TextureIntermediate");

    // 注册属性
    FTextureIntermediate::Register_FTextureIntermediate_Properties(Type);

}

void FTextureIntermediate::Z_TextureIntermediate_Register::Register_FTextureIntermediate()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FTextureIntermediate>(Register_FTextureIntermediate_Impl);
}

