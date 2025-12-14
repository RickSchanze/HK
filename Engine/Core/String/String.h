#pragma once

#include "Core/String/StringView.h"
#include "Core/Utility/Macros.h"
#include <string>

class FString
{
public:
    using SizeType = size_t;
    using Iterator = std::string::iterator;
    using ConstIterator = std::string::const_iterator;
    using ReverseIterator = std::string::reverse_iterator;
    using ConstReverseIterator = std::string::const_reverse_iterator;

    FString() = default;
    FString(const char* InStr) : MyData(InStr) {}
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
    SizeType Find(char Ch, SizeType Pos = 0) const noexcept;
    SizeType Find(const char* Str, SizeType Pos = 0) const noexcept;
    SizeType Find(const FStringView& Str, SizeType Pos = 0) const noexcept;
    SizeType Find(const FString& Str, SizeType Pos = 0) const noexcept;

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
    FString operator+(const FString& Str) const;
    FString operator+(char Ch) const;

    bool operator==(const FString& Other) const noexcept;
    bool operator!=(const FString& Other) const noexcept;
    bool operator<(const FString& Other) const noexcept;
    bool operator>(const FString& Other) const noexcept;
    bool operator<=(const FString& Other) const noexcept;
    bool operator>=(const FString& Other) const noexcept;

    operator FStringView() const noexcept
    {
        return FStringView(MyData);
    }

private:
    std::string MyData;
};
