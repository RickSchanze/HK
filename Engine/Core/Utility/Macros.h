#pragma once

// 定义宏，使枚举类支持位运算
#include <cstdint>
#define HK_ENABLE_BITMASK_OPERATORS(Enum)                                                                              \
    inline constexpr Enum operator|(Enum lhs, Enum rhs)                                                                \
    {                                                                                                                  \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) |                                      \
                                 static_cast<std::underlying_type_t<Enum>>(rhs));                                      \
    }                                                                                                                  \
    inline constexpr Enum operator&(Enum lhs, Enum rhs)                                                                \
    {                                                                                                                  \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) &                                      \
                                 static_cast<std::underlying_type_t<Enum>>(rhs));                                      \
    }                                                                                                                  \
    inline constexpr Enum operator^(Enum lhs, Enum rhs)                                                                \
    {                                                                                                                  \
        return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(lhs) ^                                      \
                                 static_cast<std::underlying_type_t<Enum>>(rhs));                                      \
    }                                                                                                                  \
    inline constexpr Enum operator~(Enum e)                                                                            \
    {                                                                                                                  \
        return static_cast<Enum>(~static_cast<std::underlying_type_t<Enum>>(e));                                       \
    }                                                                                                                  \
    inline constexpr Enum& operator|=(Enum& lhs, Enum rhs)                                                             \
    {                                                                                                                  \
        return lhs = lhs | rhs;                                                                                        \
    }                                                                                                                  \
    inline constexpr Enum& operator&=(Enum& lhs, Enum rhs)                                                             \
    {                                                                                                                  \
        return lhs = lhs & rhs;                                                                                        \
    }                                                                                                                  \
    inline constexpr Enum& operator^=(Enum& lhs, Enum rhs)                                                             \
    {                                                                                                                  \
        return lhs = lhs ^ rhs;                                                                                        \
    }                                                                                                                  \
    inline constexpr bool HasFlag(Enum value, Enum flag)                                                               \
    {                                                                                                                  \
        return (value & flag) == flag;                                                                                 \
    }                                                                                                                  \
    inline constexpr void SetFlag(Enum& value, Enum flag)                                                              \
    {                                                                                                                  \
        value |= flag;                                                                                                 \
    }                                                                                                                  \
    inline constexpr void ClearFlag(Enum& value, Enum flag)                                                            \
    {                                                                                                                  \
        value &= ~flag;                                                                                                \
    }                                                                                                                  \
    inline constexpr void ToggleFlag(Enum& value, Enum flag)                                                           \
    {                                                                                                                  \
        value ^= flag;                                                                                                 \
    }

// 定义宏，为枚举类型添加自增运算符
#define HK_ENUM_INCREMENT(EnumType)                                                                                    \
    inline EnumType& operator++(EnumType& e)                                                                           \
    {                                                                                                                  \
        e = static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(e) + 1);                               \
        return e;                                                                                                      \
    }                                                                                                                  \
    inline EnumType operator++(EnumType& e, int)                                                                       \
    {                                                                                                                  \
        EnumType temp = e;                                                                                             \
        e = static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(e) + 1);                               \
        return temp;                                                                                                   \
    }

typedef int8_t Int8;
typedef int16_t Int16;
typedef int32_t Int32;
typedef int64_t Int64;
typedef uint8_t UInt8;
typedef uint16_t UInt16;
typedef uint32_t UInt32;
typedef uint64_t UInt64;
typedef float Float;
typedef double Double;
typedef bool Bool;

// 原始Assert宏（直接使用标准库，不依赖Logger）
// 用于Container、Utility、String等基础类型
#ifdef HK_DEBUG
#include <cassert>
#include <cstdio>
#include <cstdlib>
#define HK_ASSERT_RAW(Condition)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(Condition))                                                                                              \
        {                                                                                                              \
            std::fprintf(stderr, "[ASSERT] Assertion failed: %s\n", #Condition);                                       \
            std::fprintf(stderr, "[ASSERT] File: %s, Line: %d\n", __FILE__, __LINE__);                                 \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (0)

#define HK_ASSERT_MSG_RAW(Condition, Fmt, ...)                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(Condition))                                                                                              \
        {                                                                                                              \
            std::fprintf(stderr, "[ASSERT] Assertion failed: %s - ", #Condition);                                      \
            std::fprintf(stderr, Fmt __VA_OPT__(, ) __VA_ARGS__);                                                      \
            std::fprintf(stderr, "\n[ASSERT] File: %s, Line: %d\n", __FILE__, __LINE__);                               \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (0)
#else
#define HK_ASSERT_RAW(Condition) ((void)0)
#define HK_ASSERT_MSG_RAW(Condition, Fmt, ...) ((void)0)
#endif