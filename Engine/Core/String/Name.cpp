#include "Core/String/Name.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"

FName::FName(const char* InStr)
{
    if (InStr != nullptr)
    {
        ID = GetOrCreateID(std::string(InStr));
    }
    else
    {
        ID = 0;
    }
}

FName::FName(const std::string& InStr)
{
    ID = GetOrCreateID(InStr);
}

FName::FName(const FStringView& InView)
{
    ID = GetOrCreateID(std::string(InView.Data(), InView.Size()));
}

const std::string& FName::GetStdString() const
{
    HK_ASSERT_RAW(IsValid());
    auto& Table = GetNameTableInstance();
    std::lock_guard<std::mutex> Lock(Table.Mutex);
    auto It = Table.IDToString.find(ID);
    HK_ASSERT_MSG_RAW(It != Table.IDToString.end(), "FName ID not found in name table");
    return It->second;
}

FName::FNameTable& FName::GetNameTableInstance()
{
    static FNameTable Table;
    return Table;
}

FName::FIDType FName::GetOrCreateID(const std::string& InStr)
{
    auto& Table = GetNameTableInstance();
    std::lock_guard<std::mutex> Lock(Table.Mutex);

    auto It = Table.StringToID.find(InStr);
    if (It != Table.StringToID.end())
    {
        return It->second;
    }

    FIDType NewID = Table.NextID++;
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

const std::unordered_map<FName::FIDType, std::string>& FName::GetNameTable()
{
    auto& Table = GetNameTableInstance();
    return Table.IDToString;
}

FName::operator FString() const
{
    return FString(GetStdString());
}
