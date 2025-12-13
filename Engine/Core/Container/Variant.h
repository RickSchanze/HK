#pragma once

#include <variant>


template <typename... Types>
class TVariant
{
public:
    using VariantType = std::variant<Types...>;
    static constexpr size_t Size = sizeof...(Types);

    TVariant() = default;
    template <typename T>
    TVariant(T&& Value) : Data(std::forward<T>(Value))
    {
    }

    template <typename T>
    TVariant& operator=(T&& Value)
    {
        Data = std::forward<T>(Value);
        return *this;
    }

    template <typename T>
    T* Get() noexcept
    {
        return std::get_if<T>(&Data);
    }

    template <typename T>
    const T* Get() const noexcept
    {
        return std::get_if<T>(&Data);
    }

    template <size_t Index>
    auto* Get() noexcept
    {
        return std::get_if<Index>(&Data);
    }

    template <size_t Index>
    const auto* Get() const noexcept
    {
        return std::get_if<Index>(&Data);
    }

    template <typename T>
    T& GetValue()
    {
        return std::get<T>(Data);
    }

    template <typename T>
    const T& GetValue() const
    {
        return std::get<T>(Data);
    }

    template <size_t Index>
    auto& GetValue()
    {
        return std::get<Index>(Data);
    }

    template <size_t Index>
    const auto& GetValue() const
    {
        return std::get<Index>(Data);
    }

    template <typename T>
    bool HoldsAlternative() const noexcept
    {
        return std::holds_alternative<T>(Data);
    }

    template <size_t Index>
    bool HoldsAlternative() const noexcept
    {
        return Data.index() == Index;
    }

    size_t Index() const noexcept
    {
        return Data.index();
    }

    VariantType& GetVariant() noexcept
    {
        return Data;
    }
    const VariantType& GetVariant() const noexcept
    {
        return Data;
    }

private:
    VariantType Data;
};
