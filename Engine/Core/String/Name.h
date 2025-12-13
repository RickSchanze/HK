#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

class FName
{
public:
    using IDType = uint32_t;

    FName() : ID(0) {}
    explicit FName(const char* InStr);
    explicit FName(const std::string& InStr);
    explicit FName(const class FStringView& InView);

    IDType GetID() const noexcept
    {
        return ID;
    }
    const std::string& GetString() const;
    bool IsValid() const noexcept
    {
        return ID != 0;
    }
    size_t GetHashCode() const noexcept
    {
        return std::hash<IDType>{}(ID);
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

    // 全局字符串ID表管理
    static void ClearNameTable();
    static size_t GetNameTableSize();
    static const std::unordered_map<IDType, std::string>& GetNameTable();

private:
    IDType ID;

    struct FNameTable
    {
        std::unordered_map<std::string, IDType> StringToID;
        std::unordered_map<IDType, std::string> IDToString;
        IDType NextID = 1;
        std::mutex Mutex;
    };

    static FNameTable& GetNameTableInstance();
    static IDType GetOrCreateID(const std::string& InStr);
};
