#include "Core/String/String.h" // 确保包含正确的头文件
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string_view>

// 定义空白字符集合
static constexpr const char* WHITESPACE_CHARS = " \t\n\r\f\v";

// ============================================================================
// 构造函数
// ============================================================================

FString::FString(const FStringView& InView)
    : MyData(InView.Data(), InView.Size())
{
}

// ============================================================================
// 查找与子串 (Search & Substring)
// ============================================================================

FStringView FString::SubStr(FString::SizeType Pos, FString::SizeType Count) const noexcept
{
    if (Pos >= MyData.size())
    {
        return FStringView();
    }

    SizeType ActualCount = std::min(Count, MyData.size() - Pos);
    return FStringView(MyData.data() + Pos, ActualCount);
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
    return MyData.find(Str.MyData, Pos);
}

// ============================================================================
// FindLastOf (反向查找)
// ============================================================================

FString::SizeType FString::FindLastOf(char Ch, SizeType Pos) const noexcept
{
    // 查找单个字符，使用 rfind 效率更高且语义一致
    return MyData.rfind(Ch, Pos);
}

FString::SizeType FString::FindLastOf(const char* Chars, SizeType Pos) const noexcept
{
    // 查找 Chars 集合中的任意字符
    return MyData.find_last_of(Chars, Pos);
}

FString::SizeType FString::FindLastOf(const FStringView& Chars, SizeType Pos) const noexcept
{
    // 查找 Chars 集合中的任意字符，必须传入大小以支持非 null 结尾字符串
    return MyData.find_last_of(Chars.Data(), Pos, Chars.Size());
}

FString::SizeType FString::FindLastOf(const FString& Chars, SizeType Pos) const noexcept
{
    return MyData.find_last_of(Chars.MyData, Pos);
}

// ============================================================================
// 前缀与后缀 (Starts/Ends With)
// ============================================================================

bool FString::StartsWith(const FStringView& Prefix) const noexcept
{
#if __cplusplus >= 202002L
    return MyData.starts_with(std::string_view(Prefix.Data(), Prefix.Size()));
#else
    if (Prefix.Size() > MyData.size()) return false;
    return MyData.compare(0, Prefix.Size(), Prefix.Data(), Prefix.Size()) == 0;
#endif
}

bool FString::StartsWith(char Ch) const noexcept
{
    return !MyData.empty() && MyData.front() == Ch;
}

bool FString::EndsWith(const FStringView& Suffix) const noexcept
{
#if __cplusplus >= 202002L
    return MyData.ends_with(std::string_view(Suffix.Data(), Suffix.Size()));
#else
    if (Suffix.Size() > MyData.size()) return false;
    return MyData.compare(MyData.size() - Suffix.Size(), Suffix.Size(), Suffix.Data(), Suffix.Size()) == 0;
#endif
}

bool FString::EndsWith(char Ch) const noexcept
{
    return !MyData.empty() && MyData.back() == Ch;
}

// ============================================================================
// 去除空白 (Trim)
// ============================================================================

FStringView FString::Trim() const noexcept
{
    if (MyData.empty()) return FStringView();

    const SizeType First = MyData.find_first_not_of(WHITESPACE_CHARS);
    if (First == std::string::npos) return FStringView();

    const SizeType Last = MyData.find_last_not_of(WHITESPACE_CHARS);
    return FStringView(MyData.data() + First, Last - First + 1);
}

FStringView FString::TrimStart() const noexcept
{
    if (MyData.empty()) return FStringView();

    const SizeType First = MyData.find_first_not_of(WHITESPACE_CHARS);
    if (First == std::string::npos) return FStringView();

    return FStringView(MyData.data() + First, MyData.size() - First);
}

FStringView FString::TrimEnd() const noexcept
{
    if (MyData.empty()) return FStringView();

    const SizeType Last = MyData.find_last_not_of(WHITESPACE_CHARS);
    if (Last == std::string::npos) return FStringView();

    return FStringView(MyData.data(), Last + 1);
}

FString& FString::TrimInPlace()
{
    if (MyData.empty()) return *this;

    const SizeType First = MyData.find_first_not_of(WHITESPACE_CHARS);
    if (First == std::string::npos)
    {
        MyData.clear();
        return *this;
    }

    const SizeType Last = MyData.find_last_not_of(WHITESPACE_CHARS);
    if (Last != std::string::npos && Last < MyData.size() - 1)
    {
        MyData.erase(Last + 1);
    }

    if (First > 0)
    {
        MyData.erase(0, First);
    }
    return *this;
}

FString& FString::TrimStartInPlace()
{
    const SizeType First = MyData.find_first_not_of(WHITESPACE_CHARS);
    if (First == std::string::npos)
    {
        MyData.clear();
    }
    else if (First > 0)
    {
        MyData.erase(0, First);
    }
    return *this;
}

FString& FString::TrimEndInPlace()
{
    const SizeType Last = MyData.find_last_not_of(WHITESPACE_CHARS);
    if (Last == std::string::npos)
    {
        MyData.clear();
    }
    else if (Last < MyData.size() - 1)
    {
        MyData.erase(Last + 1);
    }
    return *this;
}

// ============================================================================
// 替换 (Replace)
// ============================================================================

FString FString::Replace(const FStringView& From, const FStringView& To) const
{
    FString Result = *this;
    Result.ReplaceInPlace(From, To);
    return Result;
}

FString& FString::ReplaceInPlace(const FStringView& From, const FStringView& To)
{
    if (From.IsEmpty()) return *this;

    SizeType Pos = 0;
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

// ============================================================================
// 追加 (Append)
// ============================================================================

void FString::Append(const FStringView& Str)
{
    MyData.append(Str.Data(), Str.Size());
}

void FString::Append(const FString& Str)
{
    MyData.append(Str.MyData);
}

void FString::Append(char Ch)
{
    MyData.push_back(Ch);
}

void FString::Append(const char* Str)
{
    if (Str) MyData.append(Str);
}

FString& FString::operator+=(const FStringView& Str)
{
    Append(Str);
    return *this;
}

FString& FString::operator+=(const FString& Str)
{
    MyData += Str.MyData;
    return *this;
}

FString& FString::operator+=(char Ch)
{
    MyData += Ch;
    return *this;
}

FString& FString::operator+=(const char* Str)
{
    if (Str) MyData += Str;
    return *this;
}

FString FString::operator+(const FStringView& Str) const
{
    FString Result = *this;
    Result.Append(Str);
    return Result;
}

FString FString::operator+(char Ch) const
{
    FString Result = *this;
    Result += Ch;
    return Result;
}

// ============================================================================
// 比较 (Comparison)
// ============================================================================

bool FString::operator==(const FString& Other) const noexcept
{
    return MyData == Other.MyData;
}

bool FString::operator!=(const FString& Other) const noexcept
{
    return MyData != Other.MyData;
}

bool FString::operator<(const FString& Other) const noexcept
{
    return MyData < Other.MyData;
}

bool FString::operator>(const FString& Other) const noexcept
{
    return MyData > Other.MyData;
}

bool FString::operator<=(const FString& Other) const noexcept
{
    return MyData <= Other.MyData;
}

bool FString::operator>=(const FString& Other) const noexcept
{
    return MyData >= Other.MyData;
}