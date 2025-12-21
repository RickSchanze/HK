//
// Created by Admin on 2025/12/18.
//

#include "ConfigManager.h"
#include "Core/Logging/Logger.h"
#include "Core/Reflection/TypeManager.h"
#include "Core/Serialization/Serialization.h"
#include "RHI/RHIConfig.h"

#include <filesystem>
#include <ranges>

void FConfigManager::StartUp()
{
    GetConfig<FRHIConfig>();
}

void FConfigManager::ShutDown()
{
    SaveAllConfigs();
    
    // 清理所有配置
    for (const auto& Config : Configs | std::views::values)
    {
        if (Config != nullptr)
        {
            delete Config;
        }
    }
    Configs.Clear();
    ConfigTypes.Clear();
}

void FConfigManager::SaveAllConfigs()
{
    for (const auto& Config : Configs | std::views::values)
    {
        if (Config != nullptr)
        {
            Config->Save();
        }
    }
}

IConfig* FConfigManager::GetConfig(const FName InConfigName)
{
    if (!InConfigName.IsValid())
    {
        return nullptr;
    }

    // 先查找是否已存在
    IConfig** Found = Configs.Find(InConfigName);
    if (Found != nullptr && *Found != nullptr)
    {
        return *Found;
    }

    // 如果不存在，需要通过类型名查找类型并创建
    // 这里需要遍历所有已注册的类型，查找 ConfigName 或类名匹配的
    // 由于无法直接通过名字反查类型，这里先返回 nullptr
    // 实际使用应该通过模板版本的 GetConfig<T>()
    HK_LOG_WARN(ELogcat::Config, "GetConfig by name is not fully implemented. Use GetConfig<T>() instead.");
    return nullptr;
}

FType FConfigManager::GetConfigType(IConfig* InConfig) const
{
    if (InConfig == nullptr)
    {
        return nullptr;
    }

    const FType* Found = ConfigTypes.Find(InConfig);
    return Found != nullptr ? *Found : nullptr;
}
