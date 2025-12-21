#pragma once
#include "Core/Container/Map.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Singleton/Singleton.h"
#include "Core/String/Name.h"
#include "IConfig.h"

#include <filesystem>
#include <fstream>

class FConfigManager : public FSingleton<FConfigManager>
{
public:
    void StartUp() override;
    void ShutDown() override;

    void SaveAllConfigs();

    /**
     * 通过名字获取配置，如果不存在则创建默认的
     */
    IConfig* GetConfig(FName InConfigName);

    /**
     * 获取配置的类型信息（内部使用）
     */
    FType GetConfigType(IConfig* InConfig) const;

    /**
     * 通过类型获取配置，如果不存在则创建默认的
     */
    template <typename T>
    T* GetConfig()
    {
        static_assert(std::is_base_of_v<IConfig, T>, "T must be derived from IConfig");

        // 获取配置名称
        const FName ConfigName = GetConfigName<T>();
        if (!ConfigName.IsValid())
        {
            HK_LOG_ERROR(ELogcat::Config, "Failed to get config name for type");
            return nullptr;
        }

        // 先尝试通过名字查找
        if (IConfig** Found = Configs.Find(ConfigName); Found != nullptr && *Found != nullptr)
        {
            return static_cast<T*>(*Found);
        }

        // 如果不存在，创建新实例
        T* NewConfig = CreateConfigInstance<T>();
        if (NewConfig == nullptr)
        {
            HK_LOG_ERROR(ELogcat::Config, "Failed to create config instance for type");
            return nullptr;
        }

        // 尝试从文件加载
        if (FStringView ConfigPath = GetConfigPath<T>(); !ConfigPath.IsEmpty())
        {
            if (LoadConfigFromFile(NewConfig, ConfigPath))
            {
                HK_LOG_INFO(ELogcat::Config, "Loaded config from file: {}", ConfigPath);
            }
            else
            {
                HK_LOG_INFO(ELogcat::Config, "Config file not found, using default: {}", ConfigPath);
            }
        }

        // 保存到映射中
        Configs[ConfigName] = NewConfig;
        ConfigTypes[NewConfig] = TypeOf<T>(); // 存储类型信息

        return NewConfig;
    }

private:
    /**
     * 获取配置名称（从 ConfigName 属性或类名）
     */
    template <typename T>
    static FName GetConfigName()
    {
        const FType Type = TypeOf<T>();
        if (Type == nullptr)
        {
            return {};
        }

        // 先尝试获取 ConfigName 属性
        if (const FStringView ConfigNameAttr = Type->GetAttribute(FName("ConfigName")); !ConfigNameAttr.IsEmpty())
        {
            return FName(ConfigNameAttr);
        }

        // 如果没有 ConfigName，使用类名
        return Type->Name;
    }

    /**
     * 创建配置实例
     */
    template <typename T>
    static T* CreateConfigInstance()
    {
        const FType Type = TypeOf<T>();
        if (Type == nullptr || !Type->CanCreateInstance())
        {
            return nullptr;
        }

        return Type->CreateInstanceT<T>();
    }

    /**
     * 从文件加载配置
     */
    template <typename T>
    static bool LoadConfigFromFile(T* InConfig, const FStringView InFilePath)
    {
        if (InConfig == nullptr || InFilePath.IsEmpty())
        {
            return false;
        }

        const std::filesystem::path FilePath(InFilePath.GetStdStringView());
        if (!std::filesystem::exists(FilePath))
        {
            return false;
        }

        std::ifstream File(FilePath.string(), std::ios::binary);
        if (!File.is_open())
        {
            return false;
        }

        try
        {
            FXMLInputArchive Archive(File);
            InConfig->Serialize(Archive);
            return true;
        }
        catch (const std::exception& e)
        {
            HK_LOG_ERROR(ELogcat::Config, "Failed to load config from file {}: {}", InFilePath, e.what());
            return false;
        }
    }

    /**
     * 获取配置文件路径
     */
    template <typename T>
    static FStringView GetConfigPath()
    {
        const FType Type = TypeOf<T>();
        if (Type == nullptr)
        {
            return {};
        }

        const FStringView ConfigPathAttr = Type->GetAttribute(FName("ConfigPath"));
        if (ConfigPathAttr.IsEmpty())
        {
            return "";
        }

        return ConfigPathAttr;
    }

    TMap<FName, IConfig*> Configs;
    TMap<IConfig*, FType> ConfigTypes; // 存储配置实例到类型的映射
};
