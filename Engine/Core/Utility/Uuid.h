#pragma once
#define UUID_SYSTEM_GENERATOR
#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Macros.h"
#include "stduuid/uuid.h"

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

    template <typename Archive>
    void Read(Archive& Ar)
    {
        std::string UuidString;
        Ar(UuidString);
        const auto MyUuid = uuids::uuid::from_string(UuidString);
        HK_ASSERT_MSG(MyUuid, "Invalid UUID: {}", UuidString);
        Uuid = *MyUuid;
    }

    template <typename Archive>
    void Write(Archive& Ar) const
    {
        Ar(uuids::to_string(Uuid));
    }

    uuids::uuid Uuid;
};
