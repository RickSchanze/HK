#pragma once

#include "Core/Utility/Macros.h"
#ifdef _MSC_VER
#include <stdint.h>
#else
#include <cstdint>
#endif
#include "Core/String/String.h"
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

// 前向声明
class FStringView;

class FName
{
public:
    using FIDType = UInt32;

    FName() : ID(0) {}
    explicit FName(const char* InStr);
    explicit FName(const std::string& InStr);
    explicit FName(const FString& InStr);
    explicit FName(const FStringView& InView);

    FIDType GetID() const noexcept
    {
        return ID;
    }

    const FString& GetString() const;
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

    // 隐式转换为FString（前向声明）
    operator FString() const;
    operator FStringView() const;

    // 全局字符串ID表管理
    static void ClearNameTable();
    static size_t GetNameTableSize();
    static const std::unordered_map<FIDType, FString>& GetNameTable();

private:
    FIDType ID;

    struct FNameTable
    {
        std::unordered_map<FString, FIDType> StringToID;
        std::unordered_map<FIDType, FString> IDToString;
        FIDType NextID = 1;
        std::mutex Mutex;
    };

    static FNameTable& GetNameTableInstance();
    static FIDType GetOrCreateID(const FString& InStr);
};
