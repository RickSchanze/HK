#include "AssetImporter.generated.h"
#include "Core/Reflection/TypeManager.h"
#include "Object/AssetImporter.h"


static void Z_Register_EAssetImportOptions_Impl()
{
    // 注册枚举类型
    FTypeMutable Type = FTypeManager::Register<EAssetImportOptions>("EAssetImportOptions");

    // 注册枚举成员: None
    Type->RegisterEnumMember(EAssetImportOptions::None, "None");

    // 注册枚举成员: 0
    Type->RegisterEnumMember(EAssetImportOptions::0, "0");

}

#pragma warning(disable: 4100)  // 禁用未使用参数警告

void Z_Register_EAssetImportOptions()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<EAssetImportOptions>(Z_Register_EAssetImportOptions_Impl);
}
