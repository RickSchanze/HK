#pragma once

#include "Core/String/StringView.h"
#include "Core/Utility/Macros.h"
#include "cereal/types/string.hpp" // 必须包含，用于底层 std::string 的序列化支持
#include <string>

class HK_API FString
{
public:
    static inline auto NPos = std::string::npos;
    using SizeType = size_t;
    using Iterator = std::string::iterator;
    using ConstIterator = std::string::const_iterator;
    using ReverseIterator = std::string::reverse_iterator;
    using ConstReverseIterator = std::string::const_reverse_iterator;

    FString() = default;
    explicit FString(const char* InStr) : MyData(InStr) {}
    FString(const char* InStr, SizeType InSize) : MyData(InStr, InSize) {}
    FString(const std::string& InStr) : MyData(InStr) {}
    FString(std::string&& InStr) : MyData(std::move(InStr)) {}
    FString(SizeType InSize, char Ch) : MyData(InSize, Ch) {}
    explicit FString(const FStringView& InView);
    template <typename InputIt>
    FString(InputIt First, InputIt Last) : MyData(First, Last)
    {
    }

    Iterator begin() noexcept
    {
        return MyData.begin();
    }
    Iterator end() noexcept
    {
        return MyData.end();
    }
    ConstIterator begin() const noexcept
    {
        return MyData.begin();
    }
    ConstIterator end() const noexcept
    {
        return MyData.end();
    }
    ConstIterator cbegin() const noexcept
    {
        return MyData.cbegin();
    }
    ConstIterator cend() const noexcept
    {
        return MyData.cend();
    }
    ReverseIterator rbegin() noexcept
    {
        return MyData.rbegin();
    }
    ReverseIterator rend() noexcept
    {
        return MyData.rend();
    }
    ConstReverseIterator rbegin() const noexcept
    {
        return MyData.rbegin();
    }
    ConstReverseIterator rend() const noexcept
    {
        return MyData.rend();
    }
    ConstReverseIterator crbegin() const noexcept
    {
        return MyData.crbegin();
    }
    ConstReverseIterator crend() const noexcept
    {
        return MyData.crend();
    }

    char& operator[](SizeType Index)
    {
        HK_ASSERT_RAW(Index < MyData.size());
        return MyData[Index];
    }
    const char& operator[](SizeType Index) const
    {
        HK_ASSERT_RAW(Index < MyData.size());
        return MyData[Index];
    }
    char& At(SizeType Index)
    {
        HK_ASSERT_RAW(Index < MyData.size());
        return MyData.at(Index);
    }
    const char& At(SizeType Index) const
    {
        HK_ASSERT_RAW(Index < MyData.size());
        return MyData.at(Index);
    }
    char& Front()
    {
        HK_ASSERT_RAW(!MyData.empty());
        return MyData.front();
    }
    const char& Front() const
    {
        HK_ASSERT_RAW(!MyData.empty());
        return MyData.front();
    }
    char& Back()
    {
        HK_ASSERT_RAW(!MyData.empty());
        return MyData.back();
    }
    const char& Back() const
    {
        HK_ASSERT_RAW(!MyData.empty());
        return MyData.back();
    }
    const char* Data() const noexcept
    {
        return MyData.data();
    }
    char* Data() noexcept
    {
        return MyData.data();
    }
    const char* CStr() const noexcept
    {
        return MyData.c_str();
    }

    SizeType Size() const noexcept
    {
        return MyData.size();
    }
    SizeType Length() const noexcept
    {
        return MyData.length();
    }
    bool IsEmpty() const noexcept
    {
        return MyData.empty();
    }
    SizeType Capacity() const noexcept
    {
        return MyData.capacity();
    }
    SizeType MaxSize() const noexcept
    {
        return MyData.max_size();
    }

    void Reserve(SizeType NewCapacity)
    {
        MyData.reserve(NewCapacity);
    }
    void ShrinkToFit()
    {
        MyData.shrink_to_fit();
    }
    void Resize(SizeType NewSize)
    {
        MyData.resize(NewSize);
    }
    void Resize(SizeType NewSize, char Ch)
    {
        MyData.resize(NewSize, Ch);
    }
    void Clear() noexcept
    {
        MyData.clear();
    }

    const std::string& GetStdString() const& noexcept
    {
        return MyData;
    }
    std::string&& GetStdString() && noexcept
    {
        return std::move(MyData);
    }

    FStringView SubStr(SizeType Pos, SizeType Count = std::string::npos) const noexcept;

    // --- Find (正向查找) ---
    SizeType Find(char Ch, SizeType Pos = 0) const noexcept;
    SizeType Find(const char* Str, SizeType Pos = 0) const noexcept;
    SizeType Find(const FStringView& Str, SizeType Pos = 0) const noexcept;
    SizeType Find(const FString& Str, SizeType Pos = 0) const noexcept;

    // --- FindLastOf (反向查找字符集中的任意字符) ---
    // 查找 Ch 最后一次出现的位置
    SizeType FindLastOf(char Ch, SizeType Pos = std::string::npos) const noexcept;
    // 查找 Chars 中任意字符最后一次出现的位置
    SizeType FindLastOf(const char* Chars, SizeType Pos = std::string::npos) const noexcept;
    SizeType FindLastOf(const FStringView& Chars, SizeType Pos = std::string::npos) const noexcept;
    SizeType FindLastOf(const FString& Chars, SizeType Pos = std::string::npos) const noexcept;

    bool Contains(char Ch) const noexcept
    {
        return Find(Ch) != std::string::npos;
    }
    bool Contains(const FStringView& SubStr) const noexcept
    {
        return Find(SubStr) != std::string::npos;
    }
    bool Contains(const FString& SubStr) const noexcept
    {
        return Find(SubStr) != std::string::npos;
    }

    bool StartsWith(const FStringView& Prefix) const noexcept;
    bool StartsWith(char Ch) const noexcept;
    bool EndsWith(const FStringView& Suffix) const noexcept;
    bool EndsWith(char Ch) const noexcept;

    FStringView Trim() const noexcept;
    FStringView TrimStart() const noexcept;
    FStringView TrimEnd() const noexcept;
    FString& TrimInPlace();
    FString& TrimStartInPlace();
    FString& TrimEndInPlace();

    FString Replace(const FStringView& From, const FStringView& To) const;
    FString& ReplaceInPlace(const FStringView& From, const FStringView& To);
    FString Replace(char From, char To) const;
    FString& ReplaceInPlace(char From, char To);

    void Append(const FStringView& Str);
    void Append(const FString& Str);
    void Append(char Ch);
    void Append(const char* Str);

    FString& operator+=(const FStringView& Str);
    FString& operator+=(const FString& Str);
    FString& operator+=(char Ch);
    FString& operator+=(const char* Str);

    FString operator+(const FStringView& Str) const;
    FString operator+(char Ch) const;

    bool operator==(const FString& Other) const noexcept;
    bool operator!=(const FString& Other) const noexcept;
    bool operator<(const FString& Other) const noexcept;
    bool operator>(const FString& Other) const noexcept;
    bool operator<=(const FString& Other) const noexcept;
    bool operator>=(const FString& Other) const noexcept;

    // 与 FStringView 的比较操作符
    bool operator==(const FStringView& Other) const noexcept;
    bool operator!=(const FStringView& Other) const noexcept;
    bool operator<(const FStringView& Other) const noexcept;
    bool operator>(const FStringView& Other) const noexcept;
    bool operator<=(const FStringView& Other) const noexcept;
    bool operator>=(const FStringView& Other) const noexcept;

    // ------------------------------------------------------------------
    // Cereal 序列化适配 (Primitive 模式)
    // ------------------------------------------------------------------

    std::string WritePrimitive() const
    {
        return MyData;
    }

    void ReadPrimitive(const std::string& InStr)
    {
        MyData = InStr;
    }

private:
    std::string MyData;
};

// FString 的哈希函数支持（用于 unordered_map）
namespace std
{
template <>
struct hash<FString>
{
    size_t operator()(const FString& Str) const noexcept
    {
        return hash<std::string>{}(Str.GetStdString());
    }
};
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif