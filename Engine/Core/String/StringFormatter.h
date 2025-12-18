#pragma once

#include "Core/String/Name.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"
#include <fmt/compile.h>
#include <fmt/format.h>


// FString的fmt格式化支持
template <>
struct std::formatter<FString> : std::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const FString& Str, FormatContext& Ctx) const -> decltype(Ctx.out())
    {
        return fmt::formatter<std::string>::format(Str.GetStdString(), Ctx);
    }
};

// FStringView的fmt格式化支持
template <>
struct std::formatter<FStringView> : std::formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(const FStringView& View, FormatContext& Ctx) const -> decltype(Ctx.out())
    {
        return fmt::formatter<std::string_view>::format(View.GetStdStringView(), Ctx);
    }
};

// FName的fmt格式化支持
template <>
struct std::formatter<FName> : std::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const FName& Name, FormatContext& Ctx) const -> decltype(Ctx.out())
    {
        return fmt::formatter<std::string>::format(Name.GetStdString(), Ctx);
    }
};
