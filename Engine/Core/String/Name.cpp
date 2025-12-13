#include "Core/String/Name.h"
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

const std::string& FName::GetString() const
{
    auto& Table = GetNameTableInstance();
    std::lock_guard<std::mutex> Lock(Table.Mutex);
    auto It = Table.IDToString.find(ID);
    if (It != Table.IDToString.end())
    {
        return It->second;
    }
    static const std::string EmptyString;
    return EmptyString;
}

FName::FNameTable& FName::GetNameTableInstance()
{
    static FNameTable Table;
    return Table;
}

FName::IDType FName::GetOrCreateID(const std::string& InStr)
{
    auto& Table = GetNameTableInstance();
    std::lock_guard<std::mutex> Lock(Table.Mutex);

    auto It = Table.StringToID.find(InStr);
    if (It != Table.StringToID.end())
    {
        return It->second;
    }

    IDType NewID = Table.NextID++;
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

const std::unordered_map<FName::IDType, std::string>& FName::GetNameTable()
{
    auto& Table = GetNameTableInstance();
    return Table.IDToString;
}
