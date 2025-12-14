#pragma once

#include "Core/Utility/Macros.h"
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

class FName
{
public:
    using FIDType = UInt32;

    FName() : ID(0) {}
    explicit FName(const char* InStr);
    explicit FName(const std::string& InStr);
    explicit FName(const class FStringView& InView);

    FIDType GetID() const noexcept
    {
        return ID;
    }

    const std::string& GetStdString() const;

    bool IsValid() const noexcept
    {
        return ID != 0;
    }
    size_t GetHashCode() const noexcept
    {
        return std::hash<FIDType>{}(ID);
    }

    bool operator==(const FName& Other) const noexcept
    {
        return ID == Other.ID;
    }

    bool operator!=(const FName& Other) const noexcept
    {
        return ID != Other.ID;
    }

    bool operator<(const FName& Other) const noexcept
    {
        return ID < Other.ID;
    }

    bool operator>(const FName& Other) const noexcept
    {
        return ID > Other.ID;
    }

    bool operator<=(const FName& Other) const noexcept
    {
        return ID <= Other.ID;
    }

    bool operator>=(const FName& Other) const noexcept
    {
        return ID >= Other.ID;
    }

    // 隐式转换为FString
    operator class FString() const;

    // 全局字符串ID表管理
    static void ClearNameTable();
    static size_t GetNameTableSize();
    static const std::unordered_map<FIDType, std::string>& GetNameTable();

private:
    FIDType ID;

    struct FNameTable
    {
        std::unordered_map<std::string, FIDType> StringToID;
        std::unordered_map<FIDType, std::string> IDToString;
        FIDType NextID = 1;
        std::mutex Mutex;
    };

    static FNameTable& GetNameTableInstance();
    static FIDType GetOrCreateID(const std::string& InStr);
};
