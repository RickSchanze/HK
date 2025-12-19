#include "Core/String/Name.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"

FName::FName(const char* InStr)
{
    if (InStr != nullptr)
    {
        ID = GetOrCreateID(FString(InStr));
    }
    else
    {
        ID = 0;
    }
}

FName::FName(const std::string& InStr)
{
    ID = GetOrCreateID(FString(InStr));
}

FName::FName(const FString& InStr)
{
    ID = GetOrCreateID(InStr);
}

FName::FName(const FStringView& InView)
{
    ID = GetOrCreateID(FString(InView.Data(), InView.Size()));
}

const FString& FName::GetString() const
{
    HK_ASSERT_RAW(IsValid());
    auto& Table = GetNameTableInstance();
    std::lock_guard<std::mutex> Lock(Table.Mutex);
    const auto It = Table.IDToString.find(ID);
    HK_ASSERT_MSG_RAW(It != Table.IDToString.end(), "FName ID not found in name table");
    return It->second;
}

const std::string& FName::GetStdString() const
{
    return GetString().GetStdString();
}

FName::FNameTable& FName::GetNameTableInstance()
{
    static FNameTable Table;
    return Table;
}

FName::FIDType FName::GetOrCreateID(const FString& InStr)
{
    auto& Table = GetNameTableInstance();
    std::lock_guard<std::mutex> Lock(Table.Mutex);

    const auto It = Table.StringToID.find(InStr);
    if (It != Table.StringToID.end())
    {
        return It->second;
    }

    const FIDType NewID = Table.NextID++;
    Table.StringToID[InStr] = NewID;
    Table.IDToString[NewID] = InStr;
    return NewID;
}

void FName::ClearNameTable()
{
    auto& Table = GetNameTableInstance();
    std::lock_guard<std::mutex> Lock(Table.Mutex);
    Table.StringToID.clear();
    Table.IDToString.clear();
    Table.NextID = 1;
}

size_t FName::GetNameTableSize()
{
    auto& Table = GetNameTableInstance();
    std::lock_guard<std::mutex> Lock(Table.Mutex);
    return Table.StringToID.size();
}

const std::unordered_map<FName::FIDType, FString>& FName::GetNameTable()
{
    auto& Table = GetNameTableInstance();
    return Table.IDToString;
}

FName::operator FString() const
{
    return GetString();
}

FName::operator FStringView() const
{
    HK_ASSERT_RAW(IsValid());
    auto& Table = GetNameTableInstance();
    std::lock_guard<std::mutex> Lock(Table.Mutex);
    const auto It = Table.IDToString.find(ID);
    HK_ASSERT_MSG_RAW(It != Table.IDToString.end(), "FName ID not found in name table");
    return It->second;
}
