//
// Created by Admin on 2025/12/18.
//

#include "IConfig.h"
#include "ConfigManager.h"
#include "Core/Serialization/XMLArchive.h"
#include "Core/Logging/Logger.h"
#include <filesystem>
#include <fstream>

void IConfig::Save()
{
    PreSave();

    // 从 ConfigManager 获取类型信息
    const FConfigManager& ConfigManager = FConfigManager::GetRef();
    const FType Type = ConfigManager.GetConfigType(this);
    if (Type == nullptr)
    {
        HK_LOG_ERROR(ELogcat::Config, "Failed to get type info for config");
        PostSave();
        return;
    }

    // 获取 ConfigPath 属性
    const FStringView ConfigPathAttr = Type->GetAttribute(FName("ConfigPath"));
    if (ConfigPathAttr.IsEmpty())
    {
        HK_LOG_ERROR(ELogcat::Config, "ConfigPath attribute not found for type: {}", Type->Name.GetStdString());
        PostSave();
        return;
    }

    FStringView ConfigPath = ConfigPathAttr;

    // 确保目录存在
    const std::filesystem::path FilePath(ConfigPath.GetStdStringView());
    const std::filesystem::path DirPath = FilePath.parent_path();
    if (!DirPath.empty() && !std::filesystem::exists(DirPath))
    {
        std::filesystem::create_directories(DirPath);
    }

    // 保存到文件（使用 XML 格式）
    std::ofstream File(FilePath.string(), std::ios::binary);
    if (!File.is_open())
    {
        HK_LOG_ERROR(ELogcat::Config, "Failed to open config file for writing: {}", ConfigPath);
        PostSave();
        return;
    }

    try
    {
        FXMLOutputArchive Archive(File);
        Serialize(Archive);
        HK_LOG_INFO(ELogcat::Config, "Config saved to: {}", ConfigPath);
    }
    catch (const std::exception& e)
    {
        HK_LOG_ERROR(ELogcat::Config, "Failed to save config: {}", e.what());
    }

    PostSave();
}
