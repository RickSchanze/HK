#pragma once
#define UUID_SYSTEM_GENERATOR
#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Macros.h"
#include "stduuid/uuid.h"
#include <string> // 必须包含 string，因为 cereal minimal 交互通常基于 std::string

struct FUuid
{
    static FUuid New();
    FUuid() = default;
    explicit FUuid(const FStringView InStr)
    {
        if (const auto MyUuid = uuids::uuid::from_string(InStr.Data()))
        {
            Uuid = *MyUuid;
        }
        else
        {
            HK_LOG_ERROR(ELogcat::Uuid, "Invalid UUID: {}", InStr);
        }
    }

    bool operator==(const FUuid& Other) const
    {
        return Uuid == Other.Uuid;
    }

    bool operator!=(const FUuid& Other) const
    {
        return !(*this == Other);
    }

    bool IsValid() const
    {
        return !Uuid.is_nil();
    }

    UInt64 GetHashCode() const
    {
        return std::hash<uuids::uuid>{}(Uuid);
    }

    FString ToString() const
    {
        return uuids::to_string(Uuid);
    }

    // ------------------------------------------------------------
    // Cereal 序列化适配接口 (Primitive模式)
    // ------------------------------------------------------------

    // 1. 返回基础类型 (std::string)
    std::string WritePrimitive() const
    {
        return uuids::to_string(Uuid);
    }

    // 2. 接收基础类型 (std::string)
    void ReadPrimitive(const std::string& InStr)
    {
        // ... (解析逻辑)
        if (const auto MyUuid = uuids::uuid::from_string(InStr)) {
            Uuid = *MyUuid;
        } else {
            Uuid = uuids::uuid();
        }
    }

    uuids::uuid Uuid;
};