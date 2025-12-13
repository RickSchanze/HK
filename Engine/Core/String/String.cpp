#include "Core/String/String.h"
#include <algorithm>
#include <cctype>

FString::FString(const FStringView& InView) : MyData(std::string(InView.Data(), InView.Size())) {}

FStringView FString::SubStr(FString::SizeType Pos, FString::SizeType Count) const noexcept
{
    return FStringView(MyData.substr(Pos, Count));
}

FString::SizeType FString::Find(char Ch, FString::SizeType Pos) const noexcept
{
    return MyData.find(Ch, Pos);
}

FString::SizeType FString::Find(const char* Str, FString::SizeType Pos) const noexcept
{
    return MyData.find(Str, Pos);
}

FString::SizeType FString::Find(const FStringView& Str, FString::SizeType Pos) const noexcept
{
    return MyData.find(Str.Data(), Pos, Str.Size());
}

FString::SizeType FString::Find(const FString& Str, FString::SizeType Pos) const noexcept
{
    return MyData.find(Str.Data(), Pos);
}

bool FString::StartsWith(const FStringView& Prefix) const noexcept
{
    return MyData.starts_with(Prefix.Data());
}

bool FString::StartsWith(char Ch) const noexcept
{
    return MyData.starts_with(Ch);
}

bool FString::EndsWith(const FStringView& Suffix) const noexcept
{
    return MyData.ends_with(Suffix.Data());
}

bool FString::EndsWith(char Ch) const noexcept
{
    return MyData.ends_with(Ch);
}

FStringView FString::Trim() const noexcept
{
    if (MyData.empty())
    {
        return FStringView();
    }

    FString::SizeType Start = 0;
    FString::SizeType End = MyData.size();

    while (Start < End && std::isspace(static_cast<unsigned char>(MyData[Start])))
    {
        ++Start;
    }

    while (End > Start && std::isspace(static_cast<unsigned char>(MyData[End - 1])))
    {
        --End;
    }

    return FStringView(MyData.data() + Start, End - Start);
}

FStringView FString::TrimStart() const noexcept
{
    if (MyData.empty())
    {
        return FStringView();
    }

    FString::SizeType Start = 0;
    while (Start < MyData.size() && std::isspace(static_cast<unsigned char>(MyData[Start])))
    {
        ++Start;
    }

    return FStringView(MyData.data() + Start, MyData.size() - Start);
}

FStringView FString::TrimEnd() const noexcept
{
    if (MyData.empty())
    {
        return FStringView();
    }

    FString::SizeType End = MyData.size();
    while (End > 0 && std::isspace(static_cast<unsigned char>(MyData[End - 1])))
    {
        --End;
    }

    return FStringView(MyData.data(), End);
}

FString& FString::TrimInPlace()
{
    MyData.erase(0, MyData.find_first_not_of(" \t\n\r\f\v"));
    MyData.erase(MyData.find_last_not_of(" \t\n\r\f\v") + 1);
    return *this;
}

FString& FString::TrimStartInPlace()
{
    MyData.erase(0, MyData.find_first_not_of(" \t\n\r\f\v"));
    return *this;
}

FString& FString::TrimEndInPlace()
{
    MyData.erase(MyData.find_last_not_of(" \t\n\r\f\v") + 1);
    return *this;
}

FString FString::Replace(const FStringView& From, const FStringView& To) const
{
    FString Result = *this;
    Result.ReplaceInPlace(From, To);
    return Result;
}

FString& FString::ReplaceInPlace(const FStringView& From, const FStringView& To)
{
    if (From.IsEmpty())
    {
        return *this;
    }

    FString::SizeType Pos = 0;
    while ((Pos = MyData.find(From.Data(), Pos, From.Size())) != std::string::npos)
    {
        MyData.replace(Pos, From.Size(), To.Data(), To.Size());
        Pos += To.Size();
    }
    return *this;
}

FString FString::Replace(char From, char To) const
{
    FString Result = *this;
    Result.ReplaceInPlace(From, To);
    return Result;
}

FString& FString::ReplaceInPlace(char From, char To)
{
    std::replace(MyData.begin(), MyData.end(), From, To);
    return *this;
}

void FString::Append(const FStringView& Str)
{
    MyData.append(Str.Data(), Str.Size());
}

void FString::Append(const FString& Str)
{
    MyData.append(Str.Data());
}

void FString::Append(char Ch)
{
    MyData.push_back(Ch);
}

void FString::Append(const char* Str)
{
    MyData.append(Str);
}

FString& FString::operator+=(const FStringView& Str)
{
    Append(Str);
    return *this;
}

FString& FString::operator+=(const FString& Str)
{
    MyData += Str.Data();
    return *this;
}

FString& FString::operator+=(char Ch)
{
    MyData += Ch;
    return *this;
}

FString& FString::operator+=(const char* Str)
{
    MyData += Str;
    return *this;
}

FString FString::operator+(const FStringView& Str) const
{
    FString Result = *this;
    Result += Str;
    return Result;
}

FString FString::operator+(const FString& Str) const
{
    FString Result = *this;
    Result += Str;
    return Result;
}

FString FString::operator+(char Ch) const
{
    FString Result = *this;
    Result += Ch;
    return Result;
}

bool FString::operator==(const FString& Other) const noexcept
{
    return MyData == Other.Data();
}

bool FString::operator!=(const FString& Other) const noexcept
{
    return MyData != Other.Data();
}

bool FString::operator<(const FString& Other) const noexcept
{
    return MyData < Other.Data();
}

bool FString::operator>(const FString& Other) const noexcept
{
    return MyData > Other.Data();
}

bool FString::operator<=(const FString& Other) const noexcept
{
    return MyData <= Other.Data();
}

bool FString::operator>=(const FString& Other) const noexcept
{
    return MyData >= Other.Data();
}
