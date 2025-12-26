#pragma once

#include "Core/Utility/Macros.h"
#include <string>
#include <string_view>

// FString 构造函数前向声明
class FString;

class HK_API FStringView
{
public:
    using SizeType = size_t;
    using ConstIterator = std::string_view::const_iterator;
    using ConstReverseIterator = std::string_view::const_reverse_iterator;
    
    static constexpr SizeType NPos = std::string_view::npos;

    constexpr FStringView() = default;
    constexpr FStringView(const char* InStr) : View(InStr) {}
    constexpr FStringView(const char* InStr, SizeType InSize) : View(InStr, InSize) {}
    constexpr FStringView(const std::string& InStr) : View(InStr) {}
    constexpr explicit FStringView(std::string_view InView) : View(InView) {}


    FStringView(const FString& InStr) noexcept;

    ConstIterator begin() const noexcept
    {
        return View.begin();
    }
    ConstIterator end() const noexcept
    {
        return View.end();
    }
    ConstIterator cbegin() const noexcept
    {
        return View.cbegin();
    }
    ConstIterator cend() const noexcept
    {
        return View.cend();
    }
    ConstReverseIterator rbegin() const noexcept
    {
        return View.rbegin();
    }
    ConstReverseIterator rend() const noexcept
    {
        return View.rend();
    }
    ConstReverseIterator crbegin() const noexcept
    {
        return View.crbegin();
    }
    ConstReverseIterator crend() const noexcept
    {
        return View.crend();
    }

    char operator[](SizeType Index) const noexcept
    {
        HK_ASSERT_RAW(Index < View.size());
        return View[Index];
    }
    char At(SizeType Index) const
    {
        HK_ASSERT_RAW(Index < View.size());
        return View.at(Index);
    }
    char Front() const noexcept
    {
        HK_ASSERT_RAW(!View.empty());
        return View.front();
    }
    char Back() const noexcept
    {
        HK_ASSERT_RAW(!View.empty());
        return View.back();
    }
    const char* Data() const noexcept
    {
        return View.data();
    }
    SizeType Size() const noexcept
    {
        return View.size();
    }
    SizeType Length() const noexcept
    {
        return View.length();
    }
    bool IsEmpty() const noexcept
    {
        return View.empty();
    }

    SizeType Find(char Ch, SizeType Pos = 0) const noexcept
    {
        return View.find(Ch, Pos);
    }

    SizeType Find(const char* Str, SizeType Pos = 0) const noexcept
    {
        return View.find(Str, Pos);
    }

    SizeType Find(const FStringView& Str, SizeType Pos = 0) const noexcept
    {
        return View.find(Str.View, Pos);
    }

    SizeType FindFirstOf(char Ch, SizeType Pos = 0) const noexcept
    {
        return View.find_first_of(Ch, Pos);
    }

    SizeType FindFirstNotOf(char Ch, SizeType Pos = 0) const noexcept
    {
        return View.find_first_not_of(Ch, Pos);
    }

    SizeType FindLastOf(char Ch, SizeType Pos = std::string_view::npos) const noexcept
    {
        return View.find_last_of(Ch, Pos);
    }

    SizeType FindLastNotOf(char Ch, SizeType Pos = std::string_view::npos) const noexcept
    {
        return View.find_last_not_of(Ch, Pos);
    }

    bool StartsWith(const FStringView& Prefix) const noexcept
    {
        return View.starts_with(Prefix.View);
    }

    bool StartsWith(char Ch) const noexcept
    {
        return View.starts_with(Ch);
    }

    bool EndsWith(const FStringView& Suffix) const noexcept
    {
        return View.ends_with(Suffix.View);
    }

    bool EndsWith(char Ch) const noexcept
    {
        return View.ends_with(Ch);
    }

    bool Contains(const FStringView& SubStr) const noexcept
    {
        return Find(SubStr) != std::string_view::npos;
    }

    bool Contains(char Ch) const noexcept
    {
        return Find(Ch) != std::string_view::npos;
    }

    FStringView SubStr(SizeType Pos, SizeType Count = std::string_view::npos) const noexcept
    {
        return FStringView(View.substr(Pos, Count));
    }

    int Compare(const FStringView& Other) const noexcept
    {
        return View.compare(Other.View);
    }

    bool operator==(const FStringView& Other) const noexcept
    {
        return View == Other.View;
    }

    bool operator!=(const FStringView& Other) const noexcept
    {
        return View != Other.View;
    }

    bool operator<(const FStringView& Other) const noexcept
    {
        return View < Other.View;
    }

    bool operator>(const FStringView& Other) const noexcept
    {
        return View > Other.View;
    }

    bool operator<=(const FStringView& Other) const noexcept
    {
        return View <= Other.View;
    }

    bool operator>=(const FStringView& Other) const noexcept
    {
        return View >= Other.View;
    }

    const std::string_view& GetStdStringView() const noexcept
    {
        return View;
    }

    std::string GetStdString() const
    {
        return std::string(View);
    }

private:
    std::string_view View;
};
