#pragma once

// 定义宏，使枚举类支持位运算
#ifdef _MSC_VER
#include <stdint.h>
#else
#include <cstdint>
#endif

// MSVC 兼容的 __VA_OPT__ 替代方案
#ifdef _MSC_VER
// MSVC 不支持 __VA_OPT__，使用辅助宏
#define HK_VA_OPT_COMMA(...) , __VA_ARGS__
#else
// GCC/Clang 支持 __VA_OPT__
#define HK_VA_OPT_COMMA(...) __VA_OPT__(, __VA_ARGS__)
#endif
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

// DLL导出/导入宏
#ifdef HK_WINDOWS
#ifdef HK_BUILDING_DLL
// 构建DLL时导出
#define HK_API __declspec(dllexport)
#else
// 使用DLL时导入
#define HK_API __declspec(dllimport)
#endif
#else
// 非Windows平台，使用可见性属性（可选）
#ifdef HK_BUILDING_DLL
#define HK_API __attribute__((visibility("default")))
#else
#define HK_API
#endif
#endif

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
            std::fprintf(stderr, Fmt HK_VA_OPT_COMMA(__VA_ARGS__));                                                    \
            std::fprintf(stderr, "\n[ASSERT] File: %s, Line: %d\n", __FILE__, __LINE__);                               \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (0)
#else
#define HK_ASSERT_RAW(Condition) ((void)0)
#define HK_ASSERT_MSG_RAW(Condition, Fmt, ...) ((void)0)
#endif

// 包含 generated 头文件的宏，用于防止未使用警告
// 使用方法: #include INCLUDE_GENERATED(GfxDevice)
// 这会展开为: #include "GfxDevice.generated.h"
// 注意: 必须配合 #include 指令使用，不能单独使用
#define INCLUDE_GENERATED_HELPER(Name) #Name ".generated.h"
#define INCLUDE_GENERATED(Name) INCLUDE_GENERATED_HELPER(Name)