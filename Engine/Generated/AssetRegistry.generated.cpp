#include "AssetRegistry.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Object/AssetRegistry.h"


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FAssetImportSetting_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FAssetImportSetting>("AssetImportSetting");

}

HK_API void FAssetImportSetting::Z_AssetImportSetting_Register::Register_FAssetImportSetting()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FAssetImportSetting>(Register_FAssetImportSetting_Impl);
}

#define FAssetImportSetting_SERIALIZATION_CODE \
        // No serializable properties \


HK_DEFINE_CLASS_SERIALIZATION(FAssetImportSetting)

#undef FAssetImportSetting_SERIALIZATION_CODE


#pragma warning(disable: 4100)  // 禁用未使用参数警告


static void Register_FAssetMetaData_Impl()
{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<FAssetMetaData>("AssetMetaData");

    // 注册属性
    FAssetMetaData::Register_FAssetMetaData_Properties(Type);

}

HK_API void FAssetMetaData::Z_AssetMetaData_Register::Register_FAssetMetaData()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<FAssetMetaData>(Register_FAssetMetaData_Impl);
}

