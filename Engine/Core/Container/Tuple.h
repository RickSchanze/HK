#pragma once

#include <tuple>

template <typename... Types>
class TTuple
{
public:
    using TupleType = std::tuple<Types...>;
    static constexpr size_t Size = sizeof...(Types);

    TTuple() = default;
    template <typename... UTypes>
    explicit TTuple(UTypes&&... Args) : Data(std::forward<UTypes>(Args)...)
    {
    }

    template <size_t Index>
    auto& Get() & noexcept
    {
        return std::get<Index>(Data);
    }

    template <size_t Index>
    const auto& Get() const& noexcept
    {
        return std::get<Index>(Data);
    }

    template <size_t Index>
    auto&& Get() && noexcept
    {
        return std::get<Index>(std::move(Data));
    }

    template <size_t Index>
    const auto&& Get() const&& noexcept
    {
        return std::get<Index>(std::move(Data));
    }

    template <typename T>
    auto& Get() & noexcept
    {
        return std::get<T>(Data);
    }

    template <typename T>
    const auto& Get() const& noexcept
    {
        return std::get<T>(Data);
    }

    template <typename T>
    auto&& Get() && noexcept
    {
        return std::get<T>(std::move(Data));
    }

    template <typename T>
    const auto&& Get() const&& noexcept
    {
        return std::get<T>(std::move(Data));
    }

    TupleType& GetTuple() noexcept
    {
        return Data;
    }
    const TupleType& GetTuple() const noexcept
    {
        return Data;
    }

private:
    TupleType Data;
};
