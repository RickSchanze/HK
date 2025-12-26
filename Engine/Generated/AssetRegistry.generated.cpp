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


static void Z_Register_EAssetFileType_Impl()
{
    // 注册枚举类型
    FTypeMutable Type = FTypeManager::Register<EAssetFileType>("EAssetFileType");

    // 注册枚举成员: Unknown
    Type->RegisterEnumMember(EAssetFileType::Unknown, "Unknown");

    // 注册枚举成员: PNG
    Type->RegisterEnumMember(EAssetFileType::PNG, "PNG");

    // 注册枚举成员: JPG
    Type->RegisterEnumMember(EAssetFileType::JPG, "JPG");

    // 注册枚举成员: JPEG
    Type->RegisterEnumMember(EAssetFileType::JPEG, "JPEG");

    // 注册枚举成员: BMP
    Type->RegisterEnumMember(EAssetFileType::BMP, "BMP");

    // 注册枚举成员: TGA
    Type->RegisterEnumMember(EAssetFileType::TGA, "TGA");

    // 注册枚举成员: HDR
    Type->RegisterEnumMember(EAssetFileType::HDR, "HDR");

    // 注册枚举成员: EXR
    Type->RegisterEnumMember(EAssetFileType::EXR, "EXR");

    // 注册枚举成员: DDS
    Type->RegisterEnumMember(EAssetFileType::DDS, "DDS");

    // 注册枚举成员: KTX
    Type->RegisterEnumMember(EAssetFileType::KTX, "KTX");

    // 注册枚举成员: KTX2
    Type->RegisterEnumMember(EAssetFileType::KTX2, "KTX2");

    // 注册枚举成员: FBX
    Type->RegisterEnumMember(EAssetFileType::FBX, "FBX");

    // 注册枚举成员: OBJ
    Type->RegisterEnumMember(EAssetFileType::OBJ, "OBJ");

    // 注册枚举成员: GLTF
    Type->RegisterEnumMember(EAssetFileType::GLTF, "GLTF");

    // 注册枚举成员: GLB
    Type->RegisterEnumMember(EAssetFileType::GLB, "GLB");

    // 注册枚举成员: DAE
    Type->RegisterEnumMember(EAssetFileType::DAE, "DAE");

    // 注册枚举成员: BLEND
    Type->RegisterEnumMember(EAssetFileType::BLEND, "BLEND");

    // 注册枚举成员: X3D
    Type->RegisterEnumMember(EAssetFileType::X3D, "X3D");

    // 注册枚举成员: WAV
    Type->RegisterEnumMember(EAssetFileType::WAV, "WAV");

    // 注册枚举成员: OGG
    Type->RegisterEnumMember(EAssetFileType::OGG, "OGG");

    // 注册枚举成员: MP3
    Type->RegisterEnumMember(EAssetFileType::MP3, "MP3");

    // 注册枚举成员: FLAC
    Type->RegisterEnumMember(EAssetFileType::FLAC, "FLAC");

    // 注册枚举成员: MP4
    Type->RegisterEnumMember(EAssetFileType::MP4, "MP4");

    // 注册枚举成员: AVI
    Type->RegisterEnumMember(EAssetFileType::AVI, "AVI");

    // 注册枚举成员: MOV
    Type->RegisterEnumMember(EAssetFileType::MOV, "MOV");

    // 注册枚举成员: HLSL
    Type->RegisterEnumMember(EAssetFileType::HLSL, "HLSL");

    // 注册枚举成员: GLSL
    Type->RegisterEnumMember(EAssetFileType::GLSL, "GLSL");

    // 注册枚举成员: SLANG
    Type->RegisterEnumMember(EAssetFileType::SLANG, "SLANG");

    // 注册枚举成员: SPIRV
    Type->RegisterEnumMember(EAssetFileType::SPIRV, "SPIRV");

    // 注册枚举成员: JSON
    Type->RegisterEnumMember(EAssetFileType::JSON, "JSON");

    // 注册枚举成员: XML
    Type->RegisterEnumMember(EAssetFileType::XML, "XML");

    // 注册枚举成员: TXT
    Type->RegisterEnumMember(EAssetFileType::TXT, "TXT");

    // 注册枚举成员: Count
    Type->RegisterEnumMember(EAssetFileType::Count, "Count");

}

#pragma warning(disable: 4100)  // 禁用未使用参数警告

void Z_Register_EAssetFileType()
{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<EAssetFileType>(Z_Register_EAssetFileType_Impl);
}
